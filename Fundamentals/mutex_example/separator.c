/*-------------------------------------------------------------------------*
 *  *---									---*
 *   *---		separator.c						---*
 *    *---									---*
 *        *---									---*
 *         *---	----	----	----	----	----	----	----	----	---*
 *          *---									---*
 *           *---	Version 1.0		2017 February 7		Joseph Phillips	---*
 *            *---									---*
 *             *-------------------------------------------------------------------------*/
		
	//---		Header inclusion and namespace specification:		---//
	#include <stdio.h>
	#include <stdlib.h>
	#define TEXT_LEN 100
	#include <string.h>

char		msg[]			= "Please enter a line of text:";		

struct		Word
{
  	char*		textPtr_;
  	struct Word*	nextPtr_;
};

struct Word* obtainCommaSeparatedList (const char* text)
	{
	struct Word*    head        = (struct Word*)malloc(sizeof(struct Word));
	struct Word*	next;
	int flag = 0;	

	char* begin = text;
	char* ptr = text;

	ptr = text;

	int i = 0;

		while(i == 0){ 	//until end of line
			
			if(*ptr == ',' || *ptr == '\0')
			{
				struct Word*    temp        = (struct Word*)malloc(sizeof(struct Word));
                        	temp->textPtr_ = (char*)malloc(sizeof((ptr-begin)+1));
                        	strncpy(temp->textPtr_, begin, ((ptr-begin)+1));
				char*           cPtr    = strchr(temp->textPtr_,',');
 	        	        if  (cPtr != NULL)
        	          	      *cPtr = '\0';
				
				if(flag == 0){			
					head = temp;
					flag++;	
				}else if(flag == 1){
					next = (struct Word*)malloc(sizeof(struct Word));
					next = temp;
					head->nextPtr_ = temp;
					flag++;
								
				}else if (flag == 2){
					next->nextPtr_ = temp;
					next = (struct Word*)malloc(sizeof(struct Word));
                        		next    =  temp; 
				}		
				begin = ptr+1;
			}
			if(*ptr == NULL){i = 1;}
			ptr++;
		}
	return(head);
}


void printCommaSeparatedList (const struct Word* list)
{
	
	
		struct Word* word = (struct Word*)malloc(sizeof(struct Word));
		word = list;

	        while(word != NULL)
        {
			printf("\" %s \"\n", word->textPtr_);
			word = word->nextPtr_;
	}




}

void freeCommaSeparatedList (struct Word* list)
{
	struct Word* word = list;
	struct Word* temp;

	while(word != NULL)
	{
		temp = word->nextPtr_;
		free(word);
		word = temp;
	}
}



int             main            (int    argc,
                                 char*  argv[]
                                )
{
	struct Word*	list;
	int shouldRun = 0;

	while(shouldRun == 0)
	{
		char* text[TEXT_LEN];
		printf("%s: ", msg);
		fgets(text, TEXT_LEN, stdin);

		if(*text == '\n'){shouldRun = 1;}

		char*		cPtr	= strchr(text,'\n');
  		if  (cPtr != NULL)
    			*cPtr = '\0'; 
	
		list = obtainCommaSeparatedList(text);
		printCommaSeparatedList(list);
		freeCommaSeparatedList(list);
	}

	
return(0);
}
