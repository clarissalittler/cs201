#!/bin/bash
# build_test.sh - Test compilation of all concurrency tutorial examples

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Directories to test
DIRS=(
  "fork-examples"
  "thread-examples"
  "synchronization-examples" 
  "ipc-examples"
  "commented-examples/01-basic-processes"
  "commented-examples/02-advanced-processes"
  "commented-examples/03-basic-threads"
  "commented-examples/04-thread-synchronization"
  "commented-examples/05-ipc"
)

# Counters
TOTAL=0
SUCCESS=0
FAILED=0

# Clean existing binaries
echo -e "${YELLOW}Cleaning existing binaries...${NC}"
if [ -d "bin" ]; then
  rm -rf bin
fi
mkdir -p bin

echo -e "${YELLOW}Testing compilation of all examples...${NC}"
echo

# Loop through each directory
for dir in "${DIRS[@]}"; do
  echo -e "${YELLOW}Testing files in ${dir}...${NC}"
  
  # Get all .c files in the directory
  files=($dir/*.c)
  
  # Skip if no files found
  if [ ! -e "${files[0]}" ]; then
    echo -e "${YELLOW}No .c files found in ${dir}${NC}"
    echo
    continue
  fi
  
  # Create output directory
  mkdir -p "bin/$dir"
  
  # Compile each file
  for file in "${files[@]}"; do
    filename=$(basename "$file")
    executable="bin/${file%.c}"
    
    echo -n "  Compiling $filename... "
    TOTAL=$((TOTAL+1))
    
    # Try to compile
    gcc -Wall -Wextra -g "$file" -o "$executable" -pthread 2> /tmp/compile_error.log
    
    if [ $? -eq 0 ]; then
      echo -e "${GREEN}SUCCESS${NC}"
      SUCCESS=$((SUCCESS+1))
    else
      echo -e "${RED}FAILED${NC}"
      echo -e "${RED}    Error: $(cat /tmp/compile_error.log | head -1)${NC}"
      FAILED=$((FAILED+1))
    fi
  done
  
  echo
done

# Summary
echo -e "${YELLOW}Compilation Summary:${NC}"
echo -e "Total files tested: ${TOTAL}"
echo -e "Successfully compiled: ${GREEN}${SUCCESS}${NC}"
if [ $FAILED -gt 0 ]; then
  echo -e "Failed to compile: ${RED}${FAILED}${NC}"
  exit 1
else
  echo -e "${GREEN}All examples compiled successfully!${NC}"
  exit 0
fi