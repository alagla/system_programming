#include        "assign2Headers.h"


int	answererPid;
int	guesserPid;
int	shouldRun = 1;





void alrmHandler(int sig) {
	
	kill(answererPid, TIME_OVER_SIGNAL);
	kill(guesserPid, TIME_OVER_SIGNAL);
	shouldRun = 0;
	}

void chldHandler(int            sig,
              siginfo_t*     infoPtr,
              void*          dataPtr
                                )
	{
	pid_t pid;
	int s;

	while((pid =  waitpid(-1, NULL, WNOHANG)) > 0){
		if(infoPtr->si_pid == answererPid)
			printf("answerer returned EXIT_SUCCESS to launcher pid:%d (launcher)\n", infoPtr->si_pid);
		else
			printf("guesser returned EXIT_SUCCESS to launcher pid:%d (launcher) \n", infoPtr->si_pid);
	}
	
	shouldRun = 0;
	}


main(){   
	
	struct sigaction act;

	memset(&act, '\0', sizeof(struct sigaction));
	act.sa_handler = alrmHandler;
	
	sigaction(SIGALRM, &act, NULL);
	//sigemptyset(&chldHandler.sa_mask);
	//sigemptyset(&alrmHandler.sa_mask);
	
	act.sa_sigaction = chldHandler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGCHLD, &act, NULL);
	sigaction(SIGCONT, &act, NULL);
	
		
	
	answererPid = fork();
	
	if(answererPid == 0){
		execl(ANSWERER_PROGNAME, ANSWERER_PROGNAME, NULL);
	}
	
	char    line[LINE_LEN];
	snprintf(line,LINE_LEN,"%d",answererPid);

	guesserPid = fork();

	if(guesserPid == 0){
		execl(GUESSER_PROGNAME, GUESSER_PROGNAME, line, NULL );
	}
	
	alarm(NUM_SECONDS);
	
	while(shouldRun){
		sleep(1);
	}
	sleep(1);
	sleep(1);

	printf("launcher finished\n");
	return(EXIT_SUCCESS);
}

