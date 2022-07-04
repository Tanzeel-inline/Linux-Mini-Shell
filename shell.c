/*********************
* Assignment 2 Code
* SE 317: Operating Systems
* Kinneret Academic College
* Semester 1 5780
*********************/

#include "signal_handler.h"

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);
int cmd_environ(struct tokens *tokens);
void program_execution(char * arr_tokens[]);


/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc 
{
        cmd_fun_t *fun;
        char *cmd;
        char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
        {cmd_help, "?", "Show the help menu"},
        {cmd_exit, "exit", "Exit command shell"},
        {cmd_pwd, "pwd" , "Prints the current working directory"},
        {cmd_cd, "cd" , "Change the directory"},
        {cmd_environ, "environ" , "Prints the environment varibles"},
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) 
{
        for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
        {
                printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
        }
        return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) 
{
        exit(0);
}

/* Prints the current working directory of this shell */
int cmd_pwd(unused struct tokens *tokens) 
{
        char cwd[500];
	getcwd(cwd, 500);
	printf("%s\n", cwd);
        return 1;
}

/*Change the directory*/
int cmd_cd(struct tokens *tokens)
{
        /*Check if we have more than 2 tokens, then give error*/
        if ( tokens_get_length(tokens) != 2 )
        {
                return -1;
        }
        /*Change the path and check if valid path is given*/
        if(chdir(tokens_get_token(tokens, 1)) != 0)
	{
		printf("Path not found\n");
                return -1;
	}
        return 1;
}
int cmd_environ(unused struct tokens *tokens)
{
        extern char **environ;

	for(register int i = 0 ; environ[i] != NULL ; i ++)
	{
		printf("%s\n",environ[i]);
	}
}
void re_direction(struct tokens *tokens, int start_position)
{
        int intFD;
        int outFD;
        for ( int i = start_position ; i < tokens_get_length(tokens) ; i++ )
        {
                if ( strcmp(tokens_get_token(tokens,i),"|") == 0 )
                {
                        break;
                }
                else if ( strcmp(tokens_get_token(tokens,i),"<") == 0 )
                {
                        intFD = open(tokens_get_token(tokens,i+1),
                        O_RDONLY );
                        if ( intFD == -1 )
                        {
                                printf("File reading failed\n");
                                return;
                        }
                        //Dup2 on standard output
                        //fprintf(stdout,"Found the input redirection\n");
                        dup2(intFD,0);
                }
                else if ( strcmp(tokens_get_token(tokens,i),">") == 0 )
                {
                        outFD = open(tokens_get_token(tokens,i+1),
                        O_CREAT | O_WRONLY | O_TRUNC , 0777 );
                        if ( outFD == -1 )
                        {
                                printf("File creation failed\n");
                                return;
                        }
                        //fprintf(stdout,"Found the output redirection");
                        //Dup2 on standard output
                        dup2(outFD,1);
                }
                //fprintf(stdout,"Token is : %s\n",tokens_get_token(tokens,i));
        }
}

void set_path()
{
        char* path = getenv("PATH");
        strcat(path,":");
        
        char *cwd = malloc(500 * sizeof(char));
	getcwd(cwd, 500);
        strcat(path,cwd);
	setenv((char*)"PATH", path, 1);
        setenv((char*)"shell",cwd, 1);

}
/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) 
{
        for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
        {
                if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
                {
                        return i;
                }
        }
        return -1;
}

/* Intialization procedures for this shell */
void init_shell() 
{
        /* Our shell is connected to standard input. */
        shell_terminal = STDIN_FILENO;

        /* Check if we are running interactively */
        shell_is_interactive = isatty(shell_terminal);

        if (shell_is_interactive) 
        {
                /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
                * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
                * foreground, we'll receive a SIGCONT. */
                while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
                        kill(-shell_pgid, SIGTTIN);
                /* Saves the shell's process id */
                shell_pgid = getpid();

                /* Take control of the terminal */
                tcsetpgrp(shell_terminal, shell_pgid);

                /* Save the current termios to a variable, so it can be restored later. */
                tcgetattr(shell_terminal, &shell_tmodes);
        }
}

void program_execution(char **arr_tokens)
{
        int _index = 0;
        execv(*arr_tokens,arr_tokens);
        //Check for further tokenization here
        struct stat buffer;
        char* path = getenv("PATH");
        char* saver;
        char delimiter = ':';
        char* single_path = str_tok(path,delimiter,&_index);
        
        //printf("Here\n");
        while ( single_path )
        {
                //Joining the path
                char *complete_path = malloc(strlen(single_path) + strlen("/") + strlen(arr_tokens[0]));
                memcpy(complete_path,single_path,strlen(single_path));
                memcpy(complete_path + strlen(single_path),"/",strlen("/"));
                memcpy(complete_path + strlen(single_path) + strlen("/"),arr_tokens[0],strlen(arr_tokens[0]));
                complete_path[strlen(single_path) + strlen("/") + strlen(arr_tokens[0])] = '\0';
                if ( !stat(complete_path,&buffer) )
                {
                        strcpy(arr_tokens[0],complete_path);
                        execv(*arr_tokens,arr_tokens);
                }
                //Getting next token
                single_path = str_tok(path,delimiter,&_index);
        }
}

int main(unused int argc, unused char *argv[]) {
        init_shell();
        //Setting the path resolution
        set_path();

        //Setting child id to -1 in the start
        child = -1;
        childpgid = 0;
        //Set the signals
        signal(SIGTERM, signalhandler);
        signal(SIGINT, signalhandler);
        signal(SIGQUIT, signalhandler);
	signal(SIGSTOP, signalhandler);
        signal(SIGTSTP, signalhandler);
        signal(SIGKILL, signalhandler);
        signal(SIGCONT, signalhandler);
	signal(SIGTTOU, signalhandler);
	signal(SIGTTIN, signalhandler);
        signal(SIGCONT, signalhandler);
        signal(SIGXCPU, signalhandler);
        signal(SIGXFSZ, signalhandler);
        signal(SIGVTALRM, signalhandler);
        signal(SIGPROF, signalhandler);
        static char line[4096];
        int line_num = 0;

        /* Please only print shell prompts when standard input is not a tty */
        if (shell_is_interactive)
        fprintf(stdout, "%d: ", line_num);

        while (fgets(line, 4096, stdin)) 
        {
                /* Split our line into words. */
                struct tokens *tokens = tokenize(line);

                /*Token printing*/
                /*for ( unsigned int i = 0 ; i < tokens_get_length(tokens) ; i++ )
                {
                        printf("%s\n",tokens_get_token(tokens,i));
                }*/
                /* Find which built-in function to run. */
                int fundex = lookup(tokens_get_token(tokens, 0));

                if (fundex >= 0) 
                {
                        cmd_table[fundex].fun(tokens);
                } 
                else 
                {
                        int pid_transfer[2];
                        int standard_input = dup(0);
                        int standard_output = dup(1);

                        int in = 0, out = 1;
                        int fd[2];
                        for(register int i = 0 ; i < tokens_get_length(tokens) ; i ++)
                        {
                                pipe(fd);

                                if( find_pipe_delimiter(tokens,i) != -1)
                                {
                                        out = fd[1];
                                }
                                else
                                {
                                        out = 1;
                                }
                                pipe(pid_transfer);
                                child = fork();
                                if(child == 0)
                                {
                                        char* variable = getenv((char*)"shell");
                                        setenv((char*)"parent", variable, 1);

                                        //---------------------------------------------------
                                        //PROCESS GROUP ID OF THE CHILD 
                                        //---------------------------------------------------
                                        //signal(SIGTTOU, SIG_IGN);
                                        close(pid_transfer[0]);
                                        setpgid(getpid(), getpid());
                                        //tcsetpgrp(STDIN_FILENO, getpgid(getpid()));

                                        //Sending the process group id
                                        struct send_pgid data;
                                        data.group_pid = getpid();
                                        write(pid_transfer[1],&data,sizeof(struct send_pgid));


                                        //Check if its start of commands else get input from pipe
                                        if(in != 0)
                                        {
                                                dup2(in, 0);
                                                close(in);
                                        }
                                        //Check if it is end of command
                                        //if it is not, then redirect output to pipe
                                        if(out != 1)
                                        {
                                                dup2(out, 1);
                                                close(out);
                                        }
                                        

                                        int old = i;
                                        //Get current pipe tokens
                                        char **arr_token;
                                        copy_tokens(&arr_token,i,tokens);
                                        //Check redirection on current pipe command
                                        re_direction(tokens,old);
                                        
                                        //Execute the current pipe tokens
                                        program_execution(arr_token);
                                        fprintf(stdout,"File %s not found\n",*arr_token);
                                        exit(0);
                                }
                                else
                                {
                                        close(pid_transfer[1]);
                                        int status;
                                        struct send_pgid data;
                                        //Receive child group process id
                                        read(pid_transfer[0],&data,sizeof(struct send_pgid));
                                        childpgid = data.group_pid;
                                        waitpid(child,&status,0);
                                        //AFter child have completed its execution, set the childpgid back to 0
                                        childpgid = 0;
                                }
                                //Set child pid to -1 again, since no child exist now
                                child = -1;
                                //Changing position of i to the | 
                                if ( find_pipe_delimiter(tokens,i) != -1 )
                                {
                                        i = find_pipe_delimiter(tokens,i);
                                }
                                else
                                {
                                        i = tokens_get_length(tokens);
                                }
                                close(fd[1]);
                                in = fd[0];
                        }
                        close(fd[0]);
                        dup2(standard_output,1);
                        dup2(standard_input,0);
                        
                }

                if (shell_is_interactive)
                {
                        /* Please only print shell prompts when standard input is not a tty */
                        fprintf(stdout, "%d: ", ++line_num);
                }
                /* Clean up memory */
                tokens_destroy(tokens);
        }
        return 0;
}
