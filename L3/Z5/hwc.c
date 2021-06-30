#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

char *VERSION = "v1.0";
char *GREETING = "Hello";
char *HELP = "USAGE: hwc [option] name";

static struct option long_options[] = {
  {"capitalize",  no_argument,       0, 'C'},
  {"color",       required_argument, 0, 'c'},
  {"help",        no_argument,       0, 'h'},
  {"world",       no_argument,       0, 'w'},
  {"greeting",    required_argument, 0, 'g'},
  {"version",     no_argument,       0, 'v'},
  {0,             0,                 0,  0 }
};

void greet(int capitalize, char* color, char* name){
  if(capitalize && name[0] >= 'a')
    name[0]-=32;
  if(color && strcmp(color, "never") && (!strcmp(color, "always") || isatty(1)))
    printf("\033[0;31m%s, \033[0;33m%s\n\033[0m", GREETING, name);
  else
    printf("%s, %s\n",GREETING, name);
}

int main (int argc, char **argv){

  int capitalize = 0, world = 0; 
  int opt = 0;
  int option_index;
  char *color = "";
  
  while((opt = getopt_long(argc, argv, "Cc:hwg:v", long_options, &option_index)) >= 0){

    switch (opt){
      case 'C':
        capitalize = 1;
        break;

      case 'c':
        if(!strcmp(optarg, "never") || !strcmp(optarg, "auto") || !strcmp(optarg, "always"))
          color = optarg;
        else{
          printf("--color=[never|auto|always]\n");
          exit(1);
        }
        break;

      case 'v':
        printf("%s\n", VERSION);
        exit(0);

      case 'h':
        printf("%s\n", HELP);
        exit(0);
        break;

      case 'w':
        world = 1;
        break;

      case 'g':
        GREETING = optarg;
        break;

      default:
        printf("%s\n", HELP);
        exit(1);
      }
  }

  while(optind < argc)
    greet(capitalize, color, argv[optind++]); 
  

  if(world){
    char *name = capitalize ? "World" : "world";
    greet(capitalize, color, name);
  }
  exit(0);
}
