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
#include <unistd.h>
#include <pthread.h>

void * threadMe(void * input);

int main(){
	struct stat sb;
	char *dirChange = ".storage";
	chdir(dirChange);
	if (stat("abc.txt", &sb) == -1){
		printf("seems to have failed");
	}else{
		printf("exists");
	}

	pthread_t tid[4];
	int * first;
	int ret;

	int iter;
	for (iter = 0; iter < 4; iter++){		
		first = (int *) malloc(sizeof(int));
		*first = 7 * iter;

		ret = pthread_create(&tid[iter], NULL, threadMe, first);

		if (ret != 0){printf("thread failed");}

		
	}

	for (iter = 0; iter < 4; iter++){
		pthread_join(tid[iter], (void **)&first);
		printf("caught: %i  ", *first);
		free(first);
	}

  return 1;
}


void * threadMe(void * input){
	
	int one = *(int *)input;
	//free(input);
	printf("my value is %i", one);

	pthread_exit(input);
}







