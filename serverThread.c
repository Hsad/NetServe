/* server.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#include <fcntl.h>
#include <dirent.h>

#include <pthread.h>
#include <poll.h>

#define BUFFER_SIZE 1024

char *dotTxt = ".txt";

struct mutexPair{
	pthread_mutex_t readMutex1;
	pthread_mutex_t modMutex2;
};


struct globalFileMutex{ //mutexs first
	struct mutexPair * mutexArrayPtr[100];
	char * filenameArrayPtr[100];
};

struct passThroughData{
	struct globalFileMutex * mutexData;
	int socket;	
};


void * threadStart(void * input);


int main()
{

	//create mutex storage
	struct globalFileMutex * fileMutexData;
	fileMutexData = (struct globalFileMutex *) malloc(sizeof(struct globalFileMutex));
	//if (fileMutexData->mutexArrayPtr[0] == NULL){
	//	printf("array is null"); 
	//}
/*
	fileMutexData->filenameArrayPtr[0] = (char *)malloc(sizeof("hello"));
	char *tempText = "hello";
	strncpy(fileMutexData->filenameArrayPtr[0], tempText, sizeof("hello"));
	printf("hello? : %s", fileMutexData->filenameArrayPtr[0]);
*/
	//path for the storage file address
	//char storagePath[512];
	//strncpy (storagePath, ".storage/", 9);
	


//char *dotTxt = ".txt";



	pthread_t tid; //[25];  not saving the tid, doesnt seem useful
	int ret;

	//find current path for debugging
	/*	
	char paht[PATH_MAX];
	getwd(paht);
	if (paht != NULL){
		printf("Paht is : %s\n", paht);	
	}
*/

	//create Directory
	int dirReturn = mkdir(".storage", S_IFDIR | S_IRWXU | S_IRWXO );
	printf("%d",dirReturn);
	if (dirReturn == -1){
		if (errno == EEXIST){
			printf(".storage already exists");
		}
		else{
			printf("creating .storage failed");
		}
	}
	/*
	getwd(paht);	
	if (paht != NULL){
		printf("Paht is : %s\n", paht);	
	}*/

	char *dirChange = ".storage";
	chdir(dirChange);
	
	/*
	getwd(paht);
	if (paht != NULL){
		printf("Paht is : %s\n", paht);	
	}*/

	//Empty directory /// need to implement soemthing that fills out mutexes for the exisiting files when I remove this
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d){
		printf(" -- Deleteing contents of .storage\n");
		while ((dir = readdir(d)) != NULL){
			//printf("deleteing: %s\n", dir->d_name);			
			unlink(dir->d_name);
		}
	}
		
	


  /* Create the listener socket as TCP socket */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sock < 0 )
  {
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }

  /* socket structures */
  struct sockaddr_in server;

  server.sin_family = PF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  unsigned short port = 8127;

  /* htons() is host-to-network-short for marshalling */
  /* Internet is "big endian"; Intel is "little endian" */
  server.sin_port = htons( port );
  int len = sizeof( server );

  if ( bind( sock, (struct sockaddr *)&server, len ) < 0 )
  {
    perror( "bind() failed" );
    exit( EXIT_FAILURE );
  }

  listen( sock, 5 );   /* 5 is the max number of waiting clients */
  printf( "PARENT: Listener bound to port %d\n", port );

  struct sockaddr_in client;
  int fromlen = sizeof( client );

  int pid;
  char buffer[ BUFFER_SIZE ];


	struct passThroughData * threadStartInfo; //holds pointer to mutex Data and socket connection, to be passed to starting threads

  while ( 1 )
  {
    printf( "PARENT: Blocked on accept()\n" );
    int newsock = accept( sock, (struct sockaddr *)&client,
                          (socklen_t*)&fromlen );
    printf( "PARENT: Accepted client connection\n" );

    /* handle socket in child process */
		//rather than forking we need to make it thread
		//need to take in new connection and save the newsock var for the thread, 
		//possibly with a mutex so that it isnt lost
		//the thread should be created here

		//need to pass the thread the newsock, 
		//need to keep track of the tid, what is used and what is free
		// need to set up a while loop that tries to join the threads that are done
		//and another while loop that only calls new threads with availble tid otherwise holds.
		
		//does the thread need to return anything?
		//no?

		//thread needs a loop that keeps listening for the requests from the client
		//and then calls add, list whatever
		//the files created then need to have fuck you sabrina a few mutexs assosiated with them,
		//one to stop writings, additions, or deletions.  One to stop new readers.  one to stop 
		//add needs to be blocked by other adds, otherwise it is going to fail anyway, the file needs to be blocked till the write is complete though
		//read needs to block modifications to the file, but not other reads
		//append needs to block reading and other modifiations
		//list can happen whenever
		//delete needs to block modifications and reads
		//if something is blocking reads, it must block new reads but also wait for the old reads to stop.
		//set dontReadMutex, then set dontModMutex
		//read: dontReadMutex readMutex (closeOpen to stop incoming new reads...?) then dontModMutex ....stuff.. modMutex
		//add: dontReadMutex, dontModMutex ..add.. modMutex, readMutex
		

		//Deadlock rules, always have the same order, never lock a lower value whe you have a higher value
		
		//set lock on new reads, then lock modifications, then unlock mods, unlock reads 
		//mutex priority: read == 1, mod == 2
		//never lock read when mod is locked
		

		//readv2:  lockread, lockMod, unlock read, ........., unlock mod
		//addv2: lock read, lock mod, .....if creating file, create mutex assosiation.... unlock mod, unlock read
		//append: lock read, lock mod ...... unlock mod, unlock read
		//list: ...list files...
		//delete: lock read, lock mod .... delete file... save pointers to filename and mutexes... 
		//  set to NULL in storage... unlock mod, unlock read, free filename and mutex mem, delete done

		//need storage structure for each file, containing its two mutexes
		//is it only two mutexes?  I think so..
		//file mutexes need to be created for for currently existing files too
		
		//struct of two arrays of pointers
		//first array is pointers to filenames, space is malloced
		//second array is pointers of struct of two mutexes 
		//index of file mame corresponds to mutex index
		//created and destroyed with file
		
		threadStartInfo = (struct passThroughData *) malloc(sizeof(struct passThroughData));
		threadStartInfo->socket = newsock;
		//set pointer to mutex array


		ret = pthread_create(&tid, NULL, threadStart, threadStartInfo);
		if (ret != 0){printf("thread failed");}
		//pthread_detach(tid);

		pid = fork();

    if ( pid < 0 )
    {
      perror( "fork() failed" );
      exit( EXIT_FAILURE );
    }
    else if ( pid == 0 )
    {
      int n;

#if 0
sleep( 10 );
#endif
			/*
			do
			{
		    printf( "CHILD %d: Blocked on recv()\n", getpid() );
				printf("socket in CHILD is: %i\n", newsock);
		    // can also use read() and write()..... 
				
		    n = recv( newsock, buffer, BUFFER_SIZE, 0 );

		    if ( n < 0 )
		    {
		      perror( "recv() failed" );
		    }
				else if ( n == 0 )
				{
					printf( "CHILD %d: Rcvd 0 from recv(); closing socket\n",
		        getpid() );
				}
		    else
		    {
					//need to check if the file type is text, if so then branch to text
					//need to check spaceing to break apart buffer and get the byte number
					//strtok with space, find the command the file and the byte number
					//look at the command in this order, list, read, delete, add, append
					char bufCopy[BUFFER_SIZE];
					char dataBuffer[BUFFER_SIZE];
					strncpy(bufCopy, buffer, BUFFER_SIZE);
					strncpy(dataBuffer, buffer, BUFFER_SIZE);
					char *command = strtok(bufCopy," \n");
					char *fileName = strtok(NULL," \n");
					char *bytes = strtok(NULL," \n");
					char *dataStart = strchr(buffer, '\n');
					printf("DASH SAYS: ");
					printf("%s",command);
					printf("\nDASH NOW SAYS: ");
					printf("%s",fileName);
					printf("\nDASH FINALY SAYS: ");
					printf("%s",bytes);
					int bytesInt = atoi(bytes);
					printf("%i\n",bytesInt);
					int it;	
					for(it = 1; it < bytesInt+2; it++){
						dataBuffer[it - 1] = dataStart[it];
					}
					printf("%s", dataBuffer);  //assuming data is null termed

					if (strcmp(command,"ADD") == 0){
						printf("inside the add command\n");
						struct stat sb;
						if (stat(fileName, &sb) == -1){
							printf("file created\n");
							int fd;
							fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
							if (strstr(fileName, dotTxt) != NULL){ //file contains .txt
								write(fd, dataBuffer, bytesInt - 1); 	
							}
							else{	write(fd, dataBuffer, bytesInt );	} //not a txt file, I hope
							//need to send back ACK
							int ret = send( newsock, "ACK", 3, 0 );
							fflush( NULL );
							if ( ret != 3 ) { perror( "send() failed\n" ); }
						}
						else{
							printf("exists\n");
							//send back ERROR: FILE EXISTS
							int ret = send( newsock, "ERROR: FILE EXISTS", 18, 0 );
							fflush( NULL );
							if ( ret != 18 ) { perror( "send() failed\n" ); }
			
						}
						//need to search for the filename to see if it exists
						//if not create the file and fill in the data and return ACK, else return error
					}
		    }	
			}
			while ( n > 0 );
			*/
			
			/* this do..while loop exits when the recv() call
		 	returns 0, indicating the remote/client side has
		 	closed its socket */

      //printf( "CHILD %d: Bye!\n", getpid() );
      //close( newsock );
      exit( EXIT_SUCCESS );  /* child terminates here! */

      /* TO DO: add code to handle zombies! */
    }
    else /* pid > 0   PARENT */
    {
      /* parent simply closes the socket (endpoint) */
			printf("Parent1 closing Sock");      
			//close( newsock );
    }
  }
	printf("Parent2 closing Sock");
  close( sock );

  return EXIT_SUCCESS;
}




void * threadStart(void * input){
	
	pthread_detach(pthread_self());

	struct passThroughData inputData = *(struct passThroughData *)input;
	
	/*
	pthread_mutex_lock(one.mut);
	printf("mutex locked\n");
	fflush(NULL);
	//free(input);
	printf("my value is %i\n", one.num);
	printf("globalVal was : %i\n",globalInt);
	globalInt++;
	printf("globalVal is : %i\n",globalInt);
	pthread_mutex_unlock(one.mut);
	printf("after mutex unlock\n");
	pthread_exit(input);
	*/

	printf("Im A THREAD!!\n");
	printf("my socket is %i\n", inputData.socket);

	char inputBuffer[ BUFFER_SIZE ];
	
	//set up polling to kill thread when the client leaves
	//struct pollfd pollFileData;
	//pollFileData.fd = inputData.socket;
	//pollFileData.events = POLLIN | POLLHUP | POLLRDNORM;
	//pollFileData.revents = 0;
	
	int recvVal;

	do{
		printf("Thread recive call\n");
		fflush(NULL);
		recvVal = recv( inputData.socket, inputBuffer, BUFFER_SIZE, 0 );

		if (recvVal  < 0 )
		{
		  perror( "recv() failed" );
		}
		else if ( recvVal == 0 )
		{
			printf( "THREAD : Rcvd 0 from recv();\n");
		}
		else
		{
			//things happen here
			printf("Things happen here");
			//might want a function that takes the filename and return the mutex index, or just copy and paste like a fiend
			

	//initalizeation data, some might need to be moved elsewhere

			char bufCopy[BUFFER_SIZE];    //like this
			char dataBuffer[BUFFER_SIZE]; //and this
			strncpy(bufCopy, inputBuffer, BUFFER_SIZE);
			strncpy(dataBuffer, inputBuffer, BUFFER_SIZE);
			char *command = strtok(bufCopy," \n");
			char *fileName = strtok(NULL," \n");
			char *bytes = strtok(NULL," \n");
			char *dataStart = strchr(inputBuffer, '\n');
			/*
			printf("DASH SAYS: ");
			printf("%s",command);
			printf("\nDASH NOW SAYS: ");
			printf("%s",fileName);
			printf("\nDASH FINALY SAYS: ");
			printf("%s",bytes);
			*/
			int bytesInt = atoi(bytes);
			//printf("%i\n",bytesInt);
			int it;	
			for(it = 1; it < bytesInt+2; it++){
				dataBuffer[it - 1] = dataStart[it];
			}
			//printf("%s", dataBuffer);  //assuming data is null termed

//Mutex Aquisition, Do I need to allow ADD to run first?  Or list actually first, then ADD?
//search fileNames for mutex matching with input filename
//store index of filename
//save mutexes at that index 
//if the 

/*   Struct refrence

struct mutexPair{
	pthread_mutex_t readMutex1;
	pthread_mutex_t modMutex2;
};


struct globalFileMutex{ //mutexs first
	struct mutexPair * mutexArrayPtr[100];
	char * filenameArrayPtr[100];
};

struct passThroughData{
	struct globalFileMutex * mutexData;
	int socket;	
};


*/



//ADD	
			if (strcmp(command,"ADD") == 0){
				printf("inside the add command\n");
				struct stat sb;
				if (stat(fileName, &sb) == -1){
					//file doesnt exist
					//seek out empty location for filename and mutexes
					int loopInd;
					for( loopInd = 0; loopInd < 0; loopInd++){
						if (inputData->mutexData->filenameArrayPtr[loopInd] == NULL){
							break;
							printf("avaible file space at index: %i", loopInd);
						}
					}
					printf(" indice is still: %i :after for loop\n",loopInd);
					//setting filename into data storage
					strncpy(inputData->mutexData->filenameArrayPtr[loopInd], fileName, sizeof(fileName));
					
//need to create mutex for file
//then lock that mutex
					printf("file does not exist and will be created\n");
					int fd;
					fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
					if (strstr(fileName, dotTxt) != NULL){ //file contains .txt
						write(fd, dataBuffer, bytesInt - 1); 	
					}
					else{	write(fd, dataBuffer, bytesInt );	} //not a txt file, I hope
					int ret = send( inputData.socket, "ACK", 3, 0 );
					fflush( NULL );
					if ( ret != 3 ) { perror( "send() failed\n" ); }
				}
				else{
					printf("exists\n");
					int ret = send( inputData.socket, "ERROR: FILE EXISTS", 18, 0 );
					fflush( NULL );
					if ( ret != 18 ) { perror( "send() failed\n" ); }
	
				}
			}
//APPEND
			if (strcmp(command,"APPEND") == 0){
				printf("inside the append command\n");
				struct stat sb;
				if (stat(fileName, &sb) == -1){
					printf("file Does not exist\n");
					//need to send back ERROR: NO SUCH FILE
					int fileSize = sizeof("ERROR: NO SUCH FILE\n");
					int ret = send( inputData.socket, "ERROR: NO SUCH FILE\n", fileSize, 0 );
					fflush( NULL );					
					if ( ret != fileSize ) { perror( "send() failed\n" ); }
				}
				else{
					printf("appending to file\n");

					//need code to append to file

					//send back ACK 
					int ret = send( inputData.socket, "ACK", 3, 0 );
					fflush( NULL );
					if ( ret != 3 ) { perror( "send() failed\n" ); }	
				}
			}
//READ
			
			//if append
			//if read
			//if list
			//if delete

		}
	}while(recvVal > 0);
	printf("closing thread");

/*
	printf("Thread before the while\n");
	while(pollFileData.revents == 0){
		printf("thread in while\n");
		if(poll(&pollFileData, 1, 100) > 0){
			printf("thread in if poll\n revents: %i", pollFileData.revents);
			// >0 means data avalible or socket closed
			if(recv(inputData.socket, inputBuffer, BUFFER_SIZE, MSG_PEEK | MSG_DONTWAIT) == 0){
				//if returns zero connection has been closed
				printf(" ++ thread closing, socket was closed \n");
				pthread_exit(0);
			}
		}
	}
*/
	close(inputData.socket);
	pthread_exit(0);	
	return 0;
}







