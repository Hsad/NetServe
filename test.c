#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

int main(){
	struct stat sb;
	if (stat(".storage/abc.txt", &sb) == -1){
		printf("seems to have failed");
	}else{
		printf("exists");
	}

  return 1;
}