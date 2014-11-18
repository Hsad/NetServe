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

#define BUFFER_SIZE 1024

int main()
{
	//path for the storage file address
	//char storagePath[512];
	//strncpy (storagePath, ".storage/", 9);
	char *dotTxt = ".txt";

	char *dirChange = ".storage";
	chdir(dirChange);

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

  while ( 1 )
  {
    printf( "PARENT: Blocked on accept()\n" );
    int newsock = accept( sock, (struct sockaddr *)&client,
                          (socklen_t*)&fromlen );
    printf( "PARENT: Accepted client connection\n" );

    /* handle socket in child process */
		//rather than forking we need to make it thread

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

			do
			{
		    printf( "CHILD %d: Blocked on recv()\n", getpid() );

		    /* can also use read() and write()..... */
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
			/* this do..while loop exits when the recv() call
		 	returns 0, indicating the remote/client side has
		 	closed its socket */

      printf( "CHILD %d: Bye!\n", getpid() );
      close( newsock );
      exit( EXIT_SUCCESS );  /* child terminates here! */

      /* TO DO: add code to handle zombies! */
    }
    else /* pid > 0   PARENT */
    {
      /* parent simply closes the socket (endpoint) */
      close( newsock );
    }
  }

  close( sock );

  return EXIT_SUCCESS;
}
