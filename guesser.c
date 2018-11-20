#include        "assign2Headers.h"



int shouldRun = 1;

void            timeUpHandler   (int            sig
                                )
{
	if(sig == TIME_OVER_SIGNAL){
		printf("Oh no!  The time is up!\n");
		shouldRun	=	0;
	}
	if(sig == WIN_SIGNAL){
		printf("Congratulations!  You found it!\n");
		kill(getppid(), SIGCONT);
		shouldRun	=	0;
	}
}

void	answerHandler(int sig){
	
	if(sig == CORRECT_SIGNAL){
		printf("Yay!  That was right!\n");
	}
	if(sig == INCORRECT_SIGNAL){
		printf("Oops!  That was wrong.  Please restart from the beginning. \n");
	}
}

int		main		(int	argc,
				 char*	argv[]
				)
{

	if(argv[1] == NULL){
		printf("GUESSER GOT NO ARGS\n");
		return(EXIT_FAILURE);
	}
	int answererPid = atoi(argv[1]);
	
	struct sigaction      act;
	
	memset(&act,'\0',sizeof(act));
	act.sa_handler        = timeUpHandler;
	sigaction(TIME_OVER_SIGNAL, &act, NULL);
	sigaction(WIN_SIGNAL, &act, NULL);
	
	act.sa_handler	= answerHandler;
	sigaction(INCORRECT_SIGNAL, &act, NULL);
	sigaction(CORRECT_SIGNAL, &act, NULL);

	printf("Starting from the beginning:\n");
	
	while(shouldRun){
	
	int input = 99;

//	while((input > 1) || (input < 0)){
	
		printf("What would you like your next guess to be: 0 or 1? \n");
		scanf("%d", &input);
//	}
	
	if(input == 1)
		kill(answererPid, ONE_SIGNAL);
	if(input == 0)
		kill(answererPid, ZERO_SIGNAL);
	sleep(1);
}
	printf("guesser finished \n");
	return(EXIT_SUCCESS);
}
