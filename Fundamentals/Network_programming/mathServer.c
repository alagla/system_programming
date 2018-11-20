/*-------------------------------------------------------------------------*
 *---									---*
 *---		mathServer.c						---*
 *---									---*
 *---	    This file defines a C program that gets file-sys commands	---*
 *---	from client via a socket, executes those commands in their own	---*
 *---	threads, and returns the corresponding output back to the	---*
 *---	client.								---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2017 February 20	Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

//	Compile with:
//	$ gcc mathServer.c -o mathServer -lpthread

//---		Header file inclusion					---//

#include	"mathClientServer.h"
#include	<errno.h>	// For perror()
#include	<pthread.h>	// For pthread_create()


//---		Definition of constants:				---//

#define		STD_OKAY_MSG		"Okay"

#define		STD_ERROR_MSG		"Error doing operation"

#define		STD_BYE_MSG		"Good bye!"

#define		THIS_PROGRAM_NAME	"mathServer"

#define		FILENAME_EXTENSION	".bc"

#define		OUTPUT_FILENAME		"out.txt"

#define		ERROR_FILENAME		"err.txt"

#define		CALC_PROGNAME		"/usr/bin/bc"

const int	ERROR_FD		= -1;


//struct dirent
//{
//	ino_t d_ino; // inode number
//	off_t d_off; // offset to next dirent
//	ushort d_reclen; // length of record
//	uchar d_type; // type of file
//	char d_name[256]; // filename
//};

//---		Definition of functions:				---//

void*   dirCommand(int fd){

        DIR* dirPtr = opendir(".");
        char buffer[BUFFER_LEN];
	buffer[0] = '\0';

        if(dirPtr == NULL){ //IF FAIL TO OPEN DIR
                snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG);
        }else{

                struct dirent* entryPtr;
                char* string;

                while((entryPtr = readdir(dirPtr)) != NULL){

                        //snprintf(string, sizeof(entryPtr->d_name), "%s\n",  entryPtr->d_name);
                        strcat(buffer, entryPtr->d_name);
			strcat(buffer, "\n");
                }
        }

        closedir(dirPtr);
        write(fd,buffer,strlen(buffer)+1);
}



void* readCommand(int fd, int fileNum){
	
	char    fileName[BUFFER_LEN];
        snprintf(fileName,BUFFER_LEN,"%d%s",fileNum,FILENAME_EXTENSION);

	FILE* f = fopen(fileName, "r");
        char buffer[BUFFER_LEN];

        if(f == NULL){
                snprintf(buffer, BUFFER_LEN, "%s",  STD_ERROR_MSG);
        }else{

		if((fgets(buffer, BUFFER_LEN, f)) == NULL){
			
			snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG);
		}	
	}	

	fclose(f);
        write(fd,buffer,strlen(buffer)+1);
}

void* writeCommand(int fd, int fileNum, char* text){

	//char filename[BUFFER_LEN];
	//snprintf(filename, sizeof("%d.bc"),  "%d.bc", fileNum);
	char	fileName[BUFFER_LEN];
  	snprintf(fileName,BUFFER_LEN,"%d%s",fileNum,FILENAME_EXTENSION);	


	FILE *f = fopen(fileName, "w");
	char buffer[BUFFER_LEN];

	if(f == NULL){
		snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG); 
	}else{
		if((fprintf(f, "%s", text)) > 0){
		
			snprintf(buffer, BUFFER_LEN, "%s", STD_OKAY_MSG);
		}else{
			snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG);
		}
	}
	
	fclose(f);
	write(fd,buffer,strlen(buffer)+1);
}

void* deleteCommand(int fd, int fileNum){


	char    fileName[BUFFER_LEN];
        snprintf(fileName,BUFFER_LEN,"%d%s",fileNum,FILENAME_EXTENSION);

	char buffer[BUFFER_LEN];

	if((unlink(fileName)) == 0){
	
		snprintf(buffer, BUFFER_LEN, "%s", STD_OKAY_MSG);
	}else{
		snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG);
	}

	write(fd,buffer,strlen(buffer)+1);
}

void* calcCommand(int fd, int fileNum){

	int status;
	pid_t childId;

	if((childId = fork()) == 0){

    		char	fileName[BUFFER_LEN];
    		snprintf(fileName,BUFFER_LEN,"%d%s",fileNum,FILENAME_EXTENSION);

    		int	inFd	= open(fileName,O_RDONLY,0);
    		int	outFd	= open(OUTPUT_FILENAME,O_WRONLY|O_CREAT|O_TRUNC,0660);
    		int	errFd	= open(ERROR_FILENAME, O_WRONLY|O_CREAT|O_TRUNC,0660);

    		if  ( (inFd < 0) || (outFd < 0) || (errFd < 0) )
	    	{
      			fprintf(stderr,"Could not open one or more files\n");
      			exit(EXIT_FAILURE);
    		}

    		close(0);
    		dup(inFd);
    		close(1);
    		dup(outFd);
    		close(2);
    		dup(errFd);
				//Then run CALC_PROGNAME. If running that fails, then do:
    		execl(CALC_PROGNAME,CALC_PROGNAME, NULL);
		fprintf(stderr,"Could not execl %s\n",CALC_PROGNAME);
    		exit(EXIT_FAILURE);
	}else{
		//sleep(1);
		waitpid(childId, &status, 0);

		char buffer[BUFFER_LEN];
		
		if(WIFEXITED(status)){
			FILE* f = fopen(OUTPUT_FILENAME, "r");

		        if(f == NULL){
                		snprintf(buffer, BUFFER_LEN, "%s",  STD_ERROR_MSG);
        		}else{
                		if((fgets(buffer, BUFFER_LEN, f)) == NULL){
					snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG);
                	}
        	}

        		fclose(f);
        		write(fd,buffer,strlen(buffer)+1);
		}else{
			snprintf(buffer, BUFFER_LEN, "%s", STD_ERROR_MSG);
			write(fd,buffer,strlen(buffer)+1);
		}
	}

}

void* 	handleClient	(void* vPtr){

	int* args = (int*) vPtr;
	int fd = args[0];
	int threadNum = args[1];
	
	free(args);
	
	char	buffer[BUFFER_LEN];
	char	command;
	int	fileNum;
	char	text[BUFFER_LEN];
	int 	shouldContinue	= 1;

	while  (shouldContinue)
  	{
		memset(buffer,'\0',BUFFER_LEN);
      		memset(text  ,'\0',BUFFER_LEN);
		printf("Thread %d is in menu\n", threadNum);
    		read(fd,buffer,BUFFER_LEN);
    		printf("Thread %d received: %s\n",threadNum,buffer);
    		sscanf(buffer,"%c %d \"%[^\"]\"",&command,&fileNum,text);

    		// YOUR CODE HERE
    		//
		
		switch  (command)
    		{
    			case  	DIR_CMD_CHAR:
				dirCommand(fd);
				break;
		
			case 	READ_CMD_CHAR:	
				readCommand(fd, fileNum);
				break;

			case	WRITE_CMD_CHAR: 				
				writeCommand(fd, fileNum, text);
				break;

			case	DELETE_CMD_CHAR:
				deleteCommand(fd, fileNum);
				break;

			case	CALC_CMD_CHAR:
				calcCommand(fd,	fileNum);
				break;

			case    QUIT_CMD_CHAR:
				shouldContinue = 0;
				snprintf(buffer, BUFFER_LEN, "%s",STD_BYE_MSG);
				write(fd,buffer,strlen(buffer)+1);
                                break;
    		}
	}

  	printf("Thread %d quitting.\n",threadNum);
  	return(NULL);
}



//  PURPOSE:  To run the server by 'accept()'-ing client requests from
//	'listenFd' and doing them.
void		doServer	(int		listenFd
				)
{
  //  I.  Application validity check:

  //  II.  Server clients:
  pthread_t		threadId;
  pthread_attr_t	threadAttr;
  int			threadCount	= 0;

  // YOUR CODE HERE


  pthread_attr_init(&threadAttr);
  pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED);

  do{

	int* args = (int *)calloc(2, sizeof(int));
	args[0] = accept(listenFd, NULL, NULL);
	args[1] = threadCount++;
	
	pthread_create(&threadId,&threadAttr,handleClient,args);



  }
  while(1);
	
	pthread_attr_destroy(&threadAttr);
}


//  PURPOSE:  To decide a port number, either from the command line arguments
//	'argc' and 'argv[]', or by asking the user.  Returns port number.
int		getPortNum	(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Get listening socket:
  int	portNum;

if  (argc >= 2)
    portNum	= strtol(argv[1],NULL,0);
  else
  {
    char	buffer[BUFFER_LEN];

    printf("Port number to monopolize? ");
    fgets(buffer,BUFFER_LEN,stdin);
    portNum	= strtol(buffer,NULL,0);
  }

  //  III.  Finished:  
  return(portNum);
}


//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//	to the OS telling this server when a client process has connect()-ed
//	to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int		getServerFileDescriptor
				(int		port
				)
{
  //  I.  Application validity check:

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET, // AF_INET domain
			        SOCK_STREAM, // Reliable TCP
			        0);

  if  (socketDescriptor < 0)
  {
    perror(THIS_PROGRAM_NAME);
    return(ERROR_FD);
  }

  //  II.B.  Attempt to bind 'socketDescriptor' to 'port':
  //  II.B.1.  We'll fill in this datastruct
  struct sockaddr_in socketInfo;

  //  II.B.2.  Fill socketInfo with 0's
  memset(&socketInfo,'\0',sizeof(socketInfo));

  //  II.B.3.  Use TCP/IP:
  socketInfo.sin_family = AF_INET;

  //  II.B.4.  Tell port in network endian with htons()
  socketInfo.sin_port = htons(port);

  //  II.B.5.  Allow machine to connect to this service
  socketInfo.sin_addr.s_addr = INADDR_ANY;

  //  II.B.6.  Try to bind socket with port and other specifications
  int status = bind(socketDescriptor, // from socket()
		    (struct sockaddr*)&socketInfo,
		    sizeof(socketInfo)
		   );

  if  (status < 0)
  {
    perror(THIS_PROGRAM_NAME);
    return(ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor,5);

  //  III.  Finished:
  return(socketDescriptor);
}


int		main		(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Do server:
  int 	      port	= getPortNum(argc,argv);
  int	      listenFd	= getServerFileDescriptor(port);
  int	      status	= EXIT_FAILURE;

  if  (listenFd >= 0)
  {
    doServer(listenFd);
    close(listenFd);
    status	= EXIT_SUCCESS;
  }

  //  III.  Finished:
  return(status);
}
