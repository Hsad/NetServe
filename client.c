/* client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

//#include <time.h>
//#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main()
{
  /* create TCP client socket (endpoint) */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sock < 0 )
  {
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }

  struct hostent * hp = gethostbyname( "127.0.0.1" );
  if ( hp == NULL )
  {
    perror( "gethostbyname() failed" );
    exit( EXIT_FAILURE );
  }

  struct sockaddr_in server;
  server.sin_family = PF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr,
          hp->h_length );
  unsigned short port = 8127;
  server.sin_port = htons( port );

  printf( "server address is %s\n", inet_ntoa( server.sin_addr ) );

  if ( connect( sock, (struct sockaddr *)&server,
                sizeof( server ) ) < 0 )
  {
    perror( "connect() failed" );
    exit( EXIT_FAILURE );
  }

	char msg[1024];
	//loop
	int looper;	
	for (looper = 0; looper < 6; looper++){
		//char * msg = "heloo";
		memset(msg, 0, 1024);
		if(looper == 0){
			strncpy(msg, "ADD abc.txt 10\nabcdefghi\0", sizeof("ADD abc.txt 10\nabcdefghi\0"));
			printf("set to ADD abc.txt\n");
		} else if(looper == 1){
			printf("set to ADD xyz.txt\n");
			strncpy(msg, "ADD xyz.txt 10\nzyxwvutsr\0", strlen("ADD xyz.txt 10\nzyxwvutsr\0"));
		} else if(looper == 2){
			printf("set to LIST\n");
			strncpy(msg, "LIST\n", strlen("LIST\n"));
			//printf("msg = %s\n ---- \n", msg);
		} else if(looper == 3){
			printf("set to APPEND abc.txt\n");
			strncpy(msg, "APPEND abc.txt 10\n123456789\0", strlen("APPEND abc.txt 10\n123456789\0"));
		} else if(looper == 4){
			printf("set to READ abc.txt\n");
			strncpy(msg, "READ abc.txt\n", strlen("READ abc.txt\n"));
		} else if(looper == 5){
			printf("set to DELETE abc.txt\n");
			strncpy(msg, "DELETE abc.txt\n", strlen("DELETE abc.txt\n"));
		} 
		
		printf("about to send\n");
		fflush(NULL);
		int n = write( sock, msg, strlen( msg ) );
	fflush( NULL );
		if ( n < strlen( msg ) )
		{
		  perror( "write() failed" );
		  exit( EXIT_FAILURE );
		}
		printf("message sent\n");
		fflush(NULL);

		char buffer[ BUFFER_SIZE ];
		n = read( sock, buffer, BUFFER_SIZE );  // BLOCK
		if ( n < 0 )
		{
		  perror( "read() failed" );
		  exit( EXIT_FAILURE );
		}
		else
		{
		  buffer[n] = '\0';
		  printf( "Received message from server: %s\n", buffer );
		}
		printf("sleeping\n");
		fflush(NULL);
		sleep(2);
	}

  close( sock );

  return EXIT_SUCCESS;
}
