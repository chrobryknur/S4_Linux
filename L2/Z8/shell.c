// Marcin Dąbrowski 315370
// Lista 2
// Zadanie 8

#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#define input_buffer_size 1024
#define delimiters " \t\r\a\n"

extern char** environ;

char *msh_readline(){
  char *buffer = malloc(input_buffer_size * sizeof(char));
  char character;
  int position = 0;

  while(1){
    character = getchar();
    if(character == EOF || character == '\n'){
      buffer[position] = '\0';
      return buffer;
    }
    
    buffer[position] = character;
    position++;
  }
}


char **msh_readargs(char *line){
	char **tokens = malloc(input_buffer_size * sizeof(char*));
	char *token = strtok(line, delimiters);
	int position = 0;
	while(token){
		tokens[position] = token;
		position++;
		token = strtok(NULL, delimiters);
	}
	tokens[position] = NULL;
	return tokens;
}

void strapp(char **dstp, const char *src) {
  if (*dstp == NULL) {
    *dstp = strdup(src);
  } else {
    size_t s = strlen(*dstp) + strlen(src) + 1;
    *dstp = realloc(*dstp, s);
    strcat(*dstp, src);
  }
}


int msh_execute(char **args){
  if(args[0] == NULL)
    return 0;
  if(!strcmp(args[0], "exit"))
    return 1;
  if(!strcmp(args[0], "cd")){
    if(chdir(args[1]))
      printf("cd failed\n");
	  return 0;
  }
  pid_t pid, wpid;
  int status;

  pid = fork();
  if(!pid){
    const char *path = getenv("PATH");
    if (!index(args[0], '/') && path) { // if '/' is not in args[0] and PATH variable is available
      int i = 0;
      int length = strlen(path);
      while (i < length) {
        char *try_this_path;
        int temp_len = strcspn(path + i, ":");
        try_this_path = strndup(path + i, temp_len);
        strapp(&try_this_path, "/");
        strapp(&try_this_path, *args);
        i += temp_len + 1;
        execve(try_this_path, args, environ);
        free(try_this_path);
      }
    }
    if (execve(args[0], args, environ) == -1) {
      perror("Cannot execute command");
    }
    exit(1);
  }
  else{
    do{
      wpid = waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 0;
}


void msh_loop(){
  char *cwd = malloc(input_buffer_size * sizeof(char));
  char *line;
  char **args;
  int status = 0;

  while(!status){
    getcwd(cwd, input_buffer_size);
    /* printf("[%s]>>> ", cwd); */
    line = readline(">");
    args = msh_readargs(line);
    status = msh_execute(args);

    free(line);
    free(args);
  }
  free(cwd);
}


int main(){
  msh_loop();
  return 0;
}
