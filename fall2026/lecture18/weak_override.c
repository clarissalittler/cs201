// A strong definition of greeting(). Link this with weak_main.c and it
// replaces the weak default. See weak_main.c.

const char* greeting(void){
  return "the override from weak_override.c";
}
