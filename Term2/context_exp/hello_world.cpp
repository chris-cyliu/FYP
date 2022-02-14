#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ucontext.h>
#include <unistd.h>

#include <ctype.h>
#include <fcntl.h>

#include <sys/stat.h>

int main(int argc, char** argv) {

    char type = argv[1][0];
    
    ucontext_t ctx = {0};
    int count = 0;
    if (type == '1') {
      puts("initialzie context");
      getcontext(&ctx);   // Loop start
    } else {

      puts("restoring context from file");

      char buffer[sizeof(ctx)];
      int fileInput = open("context", O_RDONLY, 0600); 
      read(fileInput, &ctx, sizeof(ctx));
      close(fileInput);
    }

    printf("count: %d\n", count);

    sleep(1);
    
    if (++count < 3) setcontext(&ctx);   // Loop end 
    
    printf("sizeof(context): %lu\n", sizeof(ctx));

    if (type == '1') {
      puts("saving context");
      int fileOutput = open("context", O_CREAT | O_WRONLY, 0600);
      write(fileOutput, &ctx, sizeof(ctx));
      close(fileOutput);
    }

    return EXIT_SUCCESS;
}
