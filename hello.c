#include <stdio.h>
#include <unistd.h>

int main(void) {
  int increment = 0;
  while(1){
      printf(".");
      fflush(stdout);
      sleep(1);
      increment++;
  }
  return 0;
}