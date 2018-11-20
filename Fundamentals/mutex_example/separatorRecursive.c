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
	#define TEXT_LEN 1024
	#include <string.h>

char		msg[]			= "Please enter a line of text:  ";		

struct		Word
{
  	char*		textPtr_;
  	struct Word*	nextPtr_;
};

struct Word* obtainCommaSeparatedList (const char* text)
	{
	struct Word*    head;//        = (struct Word*)malloc(sizeof(struct Word));

	char* begin = text;
	char* ptr = text;

	int i = 0;

		while(i == 0){ 	//until end of line or comma
			
			if(*ptr == ',' || *ptr == '\0')
			{
				struct Word*    temp        = (struct Word*)malloc(sizeof(struct Word));
                        	temp->textPtr_ = (char*)malloc(sizeof((ptr-begin)+1));
                        	strncpy(temp->textPtr_, begin, ((ptr-begin)+1));
				char*           cPtr    = strchr(temp->textPtr_,',');
 	        	        if  (cPtr != NULL){
        	          	      *cPtr = '\0';
				}
				if(*ptr == ',')
				{	
					begin = ptr+1;
					struct Word* next = obtainCommaSeparatedList(begin);
			        	temp->nextPtr_ = next;
				}
				head = temp;

			}
			if(*ptr == '\0' || *ptr == ','){i = 1;} // stop while
			else{
				ptr++;
			}
			
		}
	return(head);
}


void printCommaSeparatedList (const struct Word* list)
{
		struct Word* word = list;
		char* z = '|';
	        while(word != NULL)
        {
			z = word->textPtr_;
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
		printf("free memory of \"%s\"\n", word->textPtr_);
		free(word->textPtr_);
		free(word);
		word = temp;
	}
}

void run()
{

	struct Word*    list;
        int shouldRun = 0;

        char* text[TEXT_LEN];
        printf("%s ", msg);
        fgets(text, TEXT_LEN, stdin);

        char*           cPtr    = strchr(text,'\n');
        
	if  (cPtr != NULL){
              *cPtr = '\0';
	}

        list = obtainCommaSeparatedList(text);
        printCommaSeparatedList(list);
        freeCommaSeparatedList(list);
        memset(text, 0, sizeof(text));

}




int             main            (int    argc,
                                 char*  argv[]
                                )
{

while(1){

	run();
}
	
return(0);
}
