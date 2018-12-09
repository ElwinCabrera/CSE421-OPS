#include <stdio.h>
#include <syscall.h>

int main (void){
    printf("create(\"\"): %d", create ("hell.dat", 0));
  return EXIT_SUCCESS;
}