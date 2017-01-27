#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ucontext.h>
#include <stdbool.h>


void saveCkptImageFile(int line_total, int fd2, FILE *fd3, ucontext_t mycontext, bool flag) {
  
  long long startAddr;
  long long endAddr;
  char startAddr1[20];
  char endAddr1[20];
  char perm[10];
  char str[1000];
  bool temp = false;
  // the data structure for a section
  struct sectiondata {
    long long startAddr;   // start address
    long long endAddr;  // end address
    bool isRegData;  // checks whether it's registered data or not  
   };
 
 while(line_total > 1) {
   int k = 0;
   int start = 0;
   int s = 0;
   fgets(str, 1000, fd3);

   // gets the start address
   for (int i=0; str[i] != '-'; i++) {
     startAddr1[i] = str[i];
     start++;
   }
   startAddr1[start] = '\0';
   
   // gets the end address
   for (int j = start+1; str[j] != ' '; j++) {
     endAddr1[k] = str[j];
     start++;
     k++;
   }
   endAddr1[k] = '\0';
   
   // gets permissions
   for (int m = start+2; str[m] != ' '; m++) {
     perm[s] = str[m];
     s++;
     start++;
   }
   perm[s] = '\0';
   line_total--;

   startAddr = strtoll(startAddr1, NULL, 16);
   endAddr = strtoll(endAddr1, NULL, 16);
   printf("Start address and end address are (%lx, %lx)\n", startAddr, endAddr);

   struct sectiondata *object = malloc(sizeof(struct sectiondata));
   object->startAddr = startAddr;
   object->endAddr = endAddr;
   object->isRegData = false;
   int sec =  write(fd2, object , sizeof(struct sectiondata));
   if (sec == -1) {
     perror("Section error");
   }
   else {
     printf("The bytes written for this section are %d\n", sec);
   }
   
   int val =  write(fd2, (void *)startAddr , endAddr - startAddr);
   if (val == -1) {
     perror("Write");
   }
   else {
     printf("The bytes written are %d\n", val);
   }
  }

   off_t offset1 = lseek(fd2, 0, SEEK_CUR ) ;
   struct sectiondata *regsec = malloc(sizeof(struct sectiondata));
   regsec->startAddr = 0;
   regsec->endAddr = 0 ;
   regsec->isRegData = true;
   write(fd2, regsec, sizeof(struct sectiondata));
  
   int val = getcontext(&mycontext);
   printf("Checking the getcontext functionality %d\n", val);
   temp = flag;
   if (temp) {
   temp = false;
   // write the context data into myckpt file
   int con = write(fd2, &mycontext, sizeof(mycontext));
   if (con == -1) {
      perror("Context error");
   }
   else {
      printf("bytes written for context are %d\n", con);
   }

  printf("The context gregs 1st and 2nd elements are (%d, %d)\n", mycontext.uc_mcontext.gregs[0], mycontext.uc_mcontext.gregs[1]);
  printf("The uc_link value is %d\n", mycontext.uc_link);
  printf("The context stack pointer is %d\n", mycontext.uc_stack.ss_sp);
  off_t offset = lseek(fd2, 0, SEEK_CUR ) ;
  printf("The last offset is %d", offset);
  }
 
}    

void signal_handler(int signum) {

   char str[1000];
   int line_total = 0;
   FILE *fd3;
   ucontext_t mycontext;
   
   int fd1 = open("/proc/self/maps", O_RDONLY);
   if (fd1 == -1) {
     perror("Maps can't be opened for reading");
   }
   
   int fd2 = open("myckpt", O_RDONLY | O_WRONLY | O_CREAT | O_APPEND, 0777); 
   if (fd2 == -1) {
      perror("Can't write to myckpt file");
   }
  
   fd3 = fdopen(fd1, "r");
   while(fgets(str, 1000, fd3) != NULL) {
      line_total++;
      printf("%s", str);
   }

   rewind(fd3);
  
   saveCkptImageFile(line_total, fd2, fd3, mycontext, true);
   
   close(fd1);
   close(fd2);
   exit(0);
}

__attribute__ ((constructor))
void myconstructor() {
  signal(SIGINT, signal_handler);
}