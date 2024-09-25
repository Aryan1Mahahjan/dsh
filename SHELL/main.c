#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


int dsh_cd(char **args);
int dsh_help(char **args);
int dsh_exit(char **args);
char *dsh_read_line(void);
char **dsh_split_line(char *line);



char *builtin_str[]= {
"cd",
"help",
"exit"
};

int (*builtin_func[])(char **) = {
	&dsh_cd,
	&dsh_help,
	&dsh_exit
};

int dsh_num_bulitins(){
return sizeof(builtin_str)/sizeof(char*);
}

int dsh_cd(char **args)
{
	if (args[1] == NULL){
		fprintf(stderr, "dsh: excepted agruement to \"cd\"\n");
	}else{
		if (chdir(args[1]) != 0)
				{
			perror("dsh");	
			}
		}
		return 1;
}

int dsh_help(char **args){
int i;
printf("Aryan Mahajan's DSH\n");
printf("Type program names and arguments\n The following are commands you can run\n");

for (i = 0; i < dsh_num_bulitins();i++){
printf(" %s\n", builtin_str[i]);
}
printf("Use the man command for informantion on other program.\n");
return 1;
}

int dsh_exit(char **args){
return 0;
}

int dsh_launch(char **args){
pid_t pid,wpid;
int status;


pid = fork();
if(pid == 0){	
	if (execvp(args[0],args) == -1){
		perror("lsg");
	}
	exit(EXIT_FAILURE);
}else if(pid < 0){
	perror("dsh");	
}else
{
	do {
		wpid = waitpid(pid, &status, WUNTRACED);
	}while(!WIFEXITED(status) && !WIFSIGNALED(status));
}
return 1;

}

int dsh_execute(char **args)
{
	int i;
	if (args[0] == NULL){
	return 1;
	}
	for (i =0; i<dsh_num_bulitins();i++){
		if (strcmp(args[0], builtin_str[i]) == 0){
			return (*builtin_func[i])(args);
		}
	}
	return dsh_launch(args);
}



void dsh_loop(void){
  char *line;
  char **args;
  int status;
  do{
    printf("> ");
    line = dsh_read_line();
    args = dsh_split_line(line);
    status = dsh_execute(args);

    free(line);
    free(args);
 }while(status);
}

#define DSH_RL_BUFSIZE 1024
char *dsh_read_line(void)
{
  int buffsize = DSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffsize);
  int c;

  if (!buffer){
    fprintf(stderr, "dsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
  
  while (1){
    c = getchar();  

    if (c == EOF || c == '\n'){
      buffer[position] = '\0';
      return buffer;
    }else{
      buffer[position] = c;
    }
    position++;

    if (position >= buffsize){
      buffsize += DSH_RL_BUFSIZE;
      buffer = realloc(buffer,buffsize);
      if (!buffer){
        fprintf(stderr, "dsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define DSH_TOK_BUFSIZE 64
#define DSH_TOK_DELIM " \t\r\n\a"
char **dsh_split_line(char *line)
{
  int bufsize = DSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens)
  {
    fprintf(stderr,"dsh : allocation error");
  }

  token = strtok(line, DSH_TOK_DELIM);
  while (token != NULL){
    tokens[position] = token;
    position++;

    if (position >= bufsize){
      bufsize += DSH_TOK_BUFSIZE;
      tokens = realloc(tokens ,bufsize * sizeof(char*));

      if (!tokens){
        fprintf(stderr, "dsh : allocation error");
        exit(EXIT_FAILURE);

      }
    }
    token = strtok(NULL, DSH_TOK_DELIM);
    }
  tokens[position] = NULL;
  return tokens;
}


int main(int argc , int **argv)
{
  // Loading config, if any
  //
  // Run command Loop .
  dsh_loop();

  //Perform any shutdown/cleanup 
}
