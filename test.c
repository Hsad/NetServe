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
#include <string.h>

int globalInt = 0;

void * threadMe(void * input);

struct passOff{
	int num;
	pthread_mutex_t *mut;
};

int main(){

	
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	struct stat sb;
	char *dirChange = ".storage";
	printf("size one: %i", (int)sizeof(dirChange));
	printf("size two: %i", (int)strlen(dirChange));
	chdir(dirChange);
	if (stat("abc.txt", &sb) == -1){
		printf("seems to have failed");
	}else{
		printf("exists");
	}

	pthread_t tid[4];
	struct passOff * first;
	int ret;



	int iter;
	for (iter = 0; iter < 4; iter++){		
		first = (struct passOff *) malloc(sizeof(struct passOff));
		first->num = iter;
		first->mut = &mutex;
		ret = pthread_create(&tid[iter], NULL, threadMe, first);
		usleep(1000);  //Apparently this is important

		if (ret != 0){printf("thread failed");}

		
	}
	/*
	for (iter = 0; iter < 4; iter++){
		pthread_join(tid[iter], (void **)&first);
		printf("caught: %i  \n", first->num);
		free(first);
	}*/
	

	pthread_mutex_destroy(&mutex);
  return 1;
}


void * threadMe(void * input){
	
	printf("just starting func");
	fflush(NULL);

	struct passOff one = *(struct passOff *)input;
	pthread_mutex_lock(one.mut);
	printf("mutex locked\n");
	fflush(NULL);
	
	printf("my value is %i\n", one.num);
	printf("globalVal was : %i\n",globalInt);
	globalInt++;
	printf("globalVal is : %i\n",globalInt);
	pthread_mutex_unlock(one.mut);
	printf("after mutex unlock\n");
	
	fflush(NULL);
	//free(input);

	pthread_exit(0);
	

	//exit(0);
	//return 0;
}







