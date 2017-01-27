#include <stdio.h>
#include <malloc.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ucontext.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
  
   char ckptfile[100];
   bool eof = false;
   ucontext_t mycontext;

   strcpy(ckptfile, argv[1]);

   struct secdata {
    long long startAddr;
    long long endAddr;
    bool isRegData;
   } section;

   int fd = open(argv[1], O_RDWR);
   if (fd == -1) {
     perror("Couldn't open myckpt file");
   }
  
   while(1) {
     if (eof == true) {
       break;
     }
     // Reads each section from myckpt
     int sec = read(fd, &section, sizeof(struct secdata)); 
     if(sec == -1) {
       perror("Read section");
     }
         
     printf("The startAddr, endAddr, and isRegData is (%lx, %lx, %d)\n", section.startAddr, section.endAddr, section.isRegData);
     off_t offset = lseek( fd, 0, SEEK_CUR ) ;
     printf("The offset is %d\n", offset);
    
     // Check if we are reading the last section
     if (section.isRegData == true) {
       printf("Reading the last section\n");
       eof = true;
       int con = read(fd, &mycontext, sizeof(mycontext));
       if (con == -1) {
          perror("Context error");
       }
       else {
          printf("The context bytes that have been read is %d\n", con);
       }
       printf("The context of first 2 elemends in gregs is (%d, %d)\n", mycontext.uc_mcontext.gregs[0], mycontext.uc_mcontext.gregs[1]);
       break;
     } else {
         char *dest = mmap((void *) section.startAddr, section.endAddr - section.startAddr, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
         if (dest == MAP_FAILED) {
            printf("Mapping failed");
         }
       
         int rd = read(fd, (void *) section.startAddr, section.endAddr - section.startAddr);
         if (rd == -1) {
	          perror("Read");
            printf("The chars read and the actual data is (%d, %d)\n", rd, section.endAddr - section.startAddr);
        }
     }
   }
   // close(fd); 
   setcontext(&mycontext);
   printf("setcontext worked");
   return 0;
   
}