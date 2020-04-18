
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

//This function will ignore ctrl+z when it is pushed
static void SIG_TSTP(int sig)
{
  signal(SIGTSTP, SIG_IGN);
}

//This function will ignore ctrl+c when it is pushed
static void SIG_INT (int sig )
{
  signal(SIGINT, SIG_IGN);
}
int main()
{
int his_count=0,pid_count=0; 
int showpid[10]={ 0,0,0,0,0,0,0,0,0,0};
char history[15][255]={ "0","0","0","0","0","0","0","0","0","0","0","0","0","0","0"};
char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
struct sigaction z;

//Zero out the sigaction struct
memset (&z, '\0', sizeof(z));


//Set the handler to use the function SIG_INT() 
z.sa_handler = SIG_TSTP;


//Install the handler and check the return value.
if (sigaction(SIGTSTP , &z, NULL) < 0)
{
	perror ("sigaction: ");
	return 1;
}
struct sigaction c;
 

//Zero out the sigaction struct
 memset (&c, '\0', sizeof(c));
 

//Set the handler to use the function SIG_INT() 
  c.sa_handler = &SIG_INT;
  
//Install the handler and check the return value.
if (sigaction(SIGINT , &c, NULL) < 0) 
{
	perror ("sigaction: ");
	return 1;
}

while( 1 )
{
// Print out the msh prompt
printf ("msh> ");

// Read the command from the commandline.  The
// maximum command that will be read is MAX_COMMAND_SIZE
// This while command will wait here until the user
// inputs something since fgets returns NULL when there
// is no input
	
	while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

/* Parse input */
char *token[MAX_NUM_ARGUMENTS];

int   token_count = 0;                                 
                                                           
// Pointer to point to the token
// parsed by strsep
char *arg_ptr;                                         
                                                           
char *working_str  = strdup( cmd_str );                

// we are going to move the working_str pointer so
// keep track of its original value so we can deallocate
// the correct amount at the end
char *working_root = working_str;

// Tokenize the input stringswith whitespace used as the delimiter
	while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		(token_count<MAX_NUM_ARGUMENTS))
	{
		token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
			if( strlen( token[token_count] ) == 0 )
			{
				token[token_count] = NULL;
			}
		token_count++;
	}

	//Checks if enter is pressed without a command
	if(token[0]==NULL)
	{
	continue; 
	}

	//This if statement is checking if '!' is the first element
	//by comparing its ASCII value, which is 33, to 33 and the same
	//samething happens to the n value too.
	char a=cmd_str[0];
	int b=a;
	if(b==33)
	{
	char a=cmd_str[1];
	int b=a-48;
	if(b>his_count)
	{
	printf("command not found in history");
	continue;
	}
	int   token_count = 0;                                                                                   
	// Pointer to point to the token
	// parsed by strsep
	char *arg_ptr;                                                
	char *working_str  = strdup( history[b-1]);                
	// we are going to move the working_str pointer so
	// keep track of its original value so we can deallocate
	// the correct amount at the end
	// Tokenize the input stringswith whitespace used as the delimiter
	while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		(token_count<MAX_NUM_ARGUMENTS))
	{
		token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
			if( strlen( token[token_count] ) == 0 )
			{
				token[token_count] = NULL;
			}
		token_count++;
	}
	}
int i;
	//In this if statement it checks to see if the number of commands that have be give
	//has exceeded 15 and if it has then the first command stored in the array will get 
	// pushed out.
	if(his_count>=15)			
	{					
	for(i=0;i<14;i++)			
	{					
	strcpy(history[i],history[i+1]);	
	}					
	strcpy(history[14],cmd_str);		
	}					
	else				
	{					
	strcpy(history[his_count],cmd_str);	
	his_count++;				 
	}					
	//Checks if history command is called and then it will print whats in history array
	 if(strcmp(token[0],"history")==0)
	{
	for(i=0;i<15;i++)
	{
	if(strcmp(history[i],"0")==0)
	{
	continue;
	}
	printf("%d %s",i+1,history[i]);
	}
	}

	
	//Checks if showpids command is called
	if(strcmp(token[0],"showpids")==0)
	{
	for(i=0;i<10;i++)
	{
	if(showpid[i]==0)
	{
	continue;
	}
	printf("%d %d\n",i+1,showpid[i]);
	}
	continue;
	}

	//Checks if exit or quit is pressed. Breaks loop if it has been.
	if(strcmp(token[0],"exit")==0||strcmp(token[0],"quit")==0)
	{
	break; 
	}

	if(strcmp(token[0],"cd")==0)
	{
	chdir(token[1] );
	continue;
	}
	
	
pid_t child_pid = fork();
int status;
	//This is the child process
	if( child_pid == 0 )
	{
		//Checks to see if the 
		//command is found or not. If not prints.
		if(execvp(token[0],token)==-1)
		{
			//checks if history is called if it is it just goes back 
			//to the top of the loop.
			if(strcmp(token[0],"history")==0)
			{
			exit( EXIT_SUCCESS );
			}
		printf("command not found.\n");
		}
	exit( EXIT_SUCCESS );
	}
	else
	{
	//parent process
	waitpid( child_pid, &status, 0 );
	//In this if statement it checks to see if the number of pids that have be stored
	//has exceeded 10 and if it has then the first pid stored in the array will get 
	// pushed out.
	if(pid_count>=10)			
	{					
		for(i=0;i<9;i++)			
		{					
		showpid[i]=showpid[i+1];	
		}					
	showpid[9]=getpid();		
	}					
	else				
	{					
	showpid[pid_count]=getppid();	
	pid_count++;			 
	}		
	}
	
	free( working_root );

}

return 0;

}

