// elfpeek: read an ELF file ourselves, no readelf allowed.
//
// An ELF file is just bytes, and <elf.h> gives us structs that describe how
// those bytes are laid out. We read the whole file into memory, then point
// structs at the right offsets:
//
//   offset 0             Elf64_Ehdr      the header: what is this file?
//   offset e_phoff       Elf64_Phdr[]    segments: what the loader maps
//   offset e_shoff       Elf64_Shdr[]    sections: what the linker works with
//
// try:
//   ./elfpeek hello
//   ./elfpeek main.o            an object file has sections but no segments
//   ./elfpeek libgreet.so
//   readelf -h -S -l hello      check our work against the real tool
//
// Every offset in the file comes from the file itself, so we check each one
// before we use it. A truncated or corrupt file shouldn't crash us.

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char* data;   // the whole file
static size_t data_len;

// Is [offset, offset + size) inside the file?
static int in_file(Elf64_Off offset, Elf64_Xword size){
  return offset <= data_len && size <= data_len - offset;
}

static int read_whole_file(const char* path){
  FILE* f = fopen(path, "rb");
  if(f == NULL){
    perror(path);
    return -1;
  }
  if(fseek(f, 0, SEEK_END) != 0){
    perror("fseek");
    fclose(f);
    return -1;
  }
  long len = ftell(f);
  if(len < 0){
    perror("ftell");
    fclose(f);
    return -1;
  }
  rewind(f);

  data_len = (size_t) len;
  data = malloc(data_len > 0 ? data_len : 1);
  if(data == NULL){
    perror("malloc");
    fclose(f);
    return -1;
  }
  if(fread(data, 1, data_len, f) != data_len){
    fprintf(stderr, "%s: couldn't read the whole file\n", path);
    fclose(f);
    return -1;
  }
  fclose(f);
  return 0;
}

static const char* file_type(Elf64_Half type){
  switch(type){
  case ET_REL:  return "REL  (object file: not linked yet)";
  case ET_EXEC: return "EXEC (executable at a fixed address)";
  case ET_DYN:  return "DYN  (shared library, or a position-independent executable)";
  case ET_CORE: return "CORE (core dump)";
  default:      return "something else";
  }
}

static const char* section_type(Elf64_Word type){
  switch(type){
  case SHT_NULL:       return "NULL";
  case SHT_PROGBITS:   return "PROGBITS";
  case SHT_NOBITS:     return "NOBITS";
  case SHT_SYMTAB:     return "SYMTAB";
  case SHT_DYNSYM:     return "DYNSYM";
  case SHT_STRTAB:     return "STRTAB";
  case SHT_RELA:       return "RELA";
  case SHT_DYNAMIC:    return "DYNAMIC";
  case SHT_NOTE:       return "NOTE";
  case SHT_INIT_ARRAY: return "INIT_ARRAY";
  case SHT_FINI_ARRAY: return "FINI_ARRAY";
  case SHT_GNU_HASH:   return "GNU_HASH";
  case SHT_GROUP:      return "GROUP";
  case SHT_GNU_versym: return "VERSYM";
  case SHT_GNU_verneed:return "VERNEED";
  default:             return "other";
  }
}

static const char* segment_type(Elf64_Word type){
  switch(type){
  case PT_LOAD:         return "LOAD";
  case PT_DYNAMIC:      return "DYNAMIC";
  case PT_INTERP:       return "INTERP";
  case PT_NOTE:         return "NOTE";
  case PT_PHDR:         return "PHDR";
  case PT_TLS:          return "TLS";
  case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
  case PT_GNU_STACK:    return "GNU_STACK";
  case PT_GNU_RELRO:    return "GNU_RELRO";
  case PT_GNU_PROPERTY: return "GNU_PROPERTY";
  default:              return "other";
  }
}

static void print_header(const Elf64_Ehdr* eh){
  printf("ELF header (the first %zu bytes)\n", sizeof(Elf64_Ehdr));
  printf("  magic          %02x %02x %02x %02x   (\\x7f E L F)\n",
         eh->e_ident[0], eh->e_ident[1], eh->e_ident[2], eh->e_ident[3]);
  printf("  class          %s\n", eh->e_ident[EI_CLASS] == ELFCLASS64 ? "64-bit" : "32-bit");
  printf("  byte order     %s\n", eh->e_ident[EI_DATA] == ELFDATA2LSB ? "little-endian" : "big-endian");
  printf("  type           %s\n", file_type(eh->e_type));
  printf("  machine        %s (%u)\n", eh->e_machine == EM_X86_64 ? "x86-64" : "not x86-64", eh->e_machine);
  printf("  entry point    %#lx\n", (unsigned long) eh->e_entry);
  printf("  segments       %u, starting at file offset %lu\n", eh->e_phnum, (unsigned long) eh->e_phoff);
  printf("  sections       %u, starting at file offset %lu\n", eh->e_shnum, (unsigned long) eh->e_shoff);
  printf("\n");
}

static void print_sections(const Elf64_Ehdr* eh){
  if(eh->e_shnum == 0){
    printf("No section headers.\n\n");
    return;
  }
  if(eh->e_shentsize != sizeof(Elf64_Shdr) ||
     !in_file(eh->e_shoff, (Elf64_Xword) eh->e_shnum * sizeof(Elf64_Shdr))){
    printf("Section headers are outside the file; skipping them.\n\n");
    return;
  }
  const Elf64_Shdr* sh = (const Elf64_Shdr*) (data + eh->e_shoff);

  // Section names aren't stored in the headers. Each header holds an offset
  // into one special string table section, and e_shstrndx says which one.
  const char* names = NULL;
  Elf64_Xword names_len = 0;
  if(eh->e_shstrndx < eh->e_shnum && in_file(sh[eh->e_shstrndx].sh_offset, sh[eh->e_shstrndx].sh_size)){
    names = (const char*) (data + sh[eh->e_shstrndx].sh_offset);
    names_len = sh[eh->e_shstrndx].sh_size;
  }

  printf("Sections (what the linker works with)\n");
  printf("  %3s  %-20s %-11s %8s %8s  %s\n", "#", "name", "type", "offset", "size", "flags");
  for(int i = 0; i < eh->e_shnum; i++){
    const char* name = "?";
    if(names != NULL && sh[i].sh_name < names_len &&
       memchr(names + sh[i].sh_name, '\0', names_len - sh[i].sh_name) != NULL){
      name = names + sh[i].sh_name;
    }
    char flags[4] = "   ";
    if(sh[i].sh_flags & SHF_ALLOC)     flags[0] = 'A';   // loaded into memory
    if(sh[i].sh_flags & SHF_WRITE)     flags[1] = 'W';
    if(sh[i].sh_flags & SHF_EXECINSTR) flags[2] = 'X';
    printf("  %3d  %-20s %-11s %8lu %8lu  %s\n", i, name, section_type(sh[i].sh_type),
           (unsigned long) sh[i].sh_offset, (unsigned long) sh[i].sh_size, flags);
  }
  printf("  (A = loaded into memory, W = writable, X = executable;\n");
  printf("   NOBITS sections like .bss take up no space in the file)\n\n");
}

static void print_segments(const Elf64_Ehdr* eh){
  if(eh->e_phnum == 0){
    printf("No segments: this file can't be run or loaded as it is.\n");
    return;
  }
  if(eh->e_phentsize != sizeof(Elf64_Phdr) ||
     !in_file(eh->e_phoff, (Elf64_Xword) eh->e_phnum * sizeof(Elf64_Phdr))){
    printf("Program headers are outside the file; skipping them.\n");
    return;
  }
  const Elf64_Phdr* ph = (const Elf64_Phdr*) (data + eh->e_phoff);

  printf("Segments (what the loader maps into memory)\n");
  printf("  %-13s %8s %11s %8s %8s  %s\n", "type", "offset", "vaddr", "filesz", "memsz", "perms");
  for(int i = 0; i < eh->e_phnum; i++){
    char perms[4] = "---";
    if(ph[i].p_flags & PF_R) perms[0] = 'r';
    if(ph[i].p_flags & PF_W) perms[1] = 'w';
    if(ph[i].p_flags & PF_X) perms[2] = 'x';
    printf("  %-13s %8lu   0x%06lx %8lu %8lu  %s", segment_type(ph[i].p_type),
           (unsigned long) ph[i].p_offset, (unsigned long) ph[i].p_vaddr,
           (unsigned long) ph[i].p_filesz, (unsigned long) ph[i].p_memsz, perms);

    // The INTERP segment holds the path of the dynamic loader.
    if(ph[i].p_type == PT_INTERP && ph[i].p_filesz > 0 && in_file(ph[i].p_offset, ph[i].p_filesz)){
      const char* path = (const char*) (data + ph[i].p_offset);
      printf("   %.*s", (int) strnlen(path, ph[i].p_filesz), path);
    }
    printf("\n");
  }
  printf("  (memsz > filesz means the rest is zero-filled: that's .bss)\n");
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <elf-file>\n", argv[0]);
    return 1;
  }
  if(read_whole_file(argv[1]) != 0){
    return 1;
  }

  if(data_len < sizeof(Elf64_Ehdr) || memcmp(data, ELFMAG, SELFMAG) != 0){
    fprintf(stderr, "%s: not an ELF file (no \\x7fELF at the start)\n", argv[1]);
    free(data);
    return 1;
  }
  const Elf64_Ehdr* eh = (const Elf64_Ehdr*) data;
  if(eh->e_ident[EI_CLASS] != ELFCLASS64 || eh->e_ident[EI_DATA] != ELFDATA2LSB){
    fprintf(stderr, "%s: elfpeek only reads 64-bit little-endian ELF files\n", argv[1]);
    free(data);
    return 1;
  }

  print_header(eh);
  print_sections(eh);
  print_segments(eh);

  free(data);
  return 0;
}
