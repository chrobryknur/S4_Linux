#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

char *VERSION = "v1.0";
char *HELP = "USAGE: mystat [option]";

const char *proc_stat_file_path = "/proc/stat";
char log_file_path[100] = "/var/log/mystat.log";

int period = 1;
int interval = 60;

double last_idle = 0;
double last_sum = 0;


static struct option long_options[] = {
  {"period",      required_argument, 0, 'p'},
  {"interval",    required_argument, 0, 'i'},
  {"log_file",    required_argument, 0, 'f'},
  {"help",        no_argument,       0, 'h'},
  {"version",     no_argument,       0, 'v'},
  {0,             0,                 0,  0 }
};

struct cpu_stats{
  int user;
  int user_nice;
  int system;
  int idle;
  int iowait;
  int irq;
  int softirq;
  int steal;
  int guest;
  int guest_nice;
};

struct cpu_usage{
  double minimal;
  double maximal;
  double average;
};

void parse_arguments(int argc, char **argv){
  int opt = 0;
  int option_index;

  while((opt = getopt_long(argc, argv, "p:i:f:hv", long_options, &option_index)) >= 0){

    switch (opt){
      case 'p':
        period = atoi(optarg);
        break;

      case 'i':
        interval = atoi(optarg);
        break;

      case 'f':
        strcpy(log_file_path, optarg);
        break;
        
      case 'v':
        printf("%s\n", VERSION);
        exit(0);

      case 'h':
        printf("%s\n", HELP);
        exit(0);
        break;

      default:
        printf("%s\n", HELP);
        exit(1);
      }
  }
}

int check_arguments(){
  FILE *proc_stat_file = fopen(proc_stat_file_path, "r");
  FILE *log_file       = fopen(log_file_path,  "a");

  if(!proc_stat_file){
    fprintf(stderr, "Couldn't open file: %s\n (probably missing permissions)", proc_stat_file_path);
    return 0;
  } 

  if(!log_file){
    fprintf(stderr, "Couldn't open file: %s\n (probably missing permissions)", log_file_path);
    return 0;
  }

  fclose(proc_stat_file);
  fclose(log_file);
  return 1;
}

int iterations_per_interval(){
  return interval / period;
}

double calculate_average_usage (struct cpu_stats stats){

  double sum = 0.0;
  sum += stats.user;
  sum += stats.user_nice;
  sum += stats.system;
  sum += stats.idle;
  sum += stats.iowait;
  sum += stats.irq;
  sum += stats.softirq;
  sum += stats.steal;
  sum += stats.guest;
  sum += stats.guest_nice;


  double avg = (1.0-((stats.idle - last_idle)/(sum - last_sum)))*100.0;

  last_idle = stats.idle;
  last_sum  = sum; 

  return avg;
}

struct cpu_usage interval_statistics(struct cpu_stats *stats){
  struct cpu_usage response;
  response.minimal = 100;
  response.maximal = 0;

  double average_usage = 0.0;

  for(int i=0;i<iterations_per_interval();i++){
    double usage = calculate_average_usage(stats[i]);      
    if(usage < response.minimal){
      response.minimal = usage;
    }    
    if(usage > response.maximal){
      response.maximal = usage;
    }
    average_usage += usage;
  }

  response.average = average_usage/iterations_per_interval();

  return response;

}


int main (int argc, char **argv){

  parse_arguments(argc, argv);

  if(!check_arguments()){
    fprintf(stderr, "%s\n",HELP);
    exit(0);
  }

  struct cpu_stats statistics[iterations_per_interval()];

  int iterations = 0;

  char line[255];


  while(1){
    FILE *proc_stat_file = fopen(proc_stat_file_path, "r");
    FILE *log_file       = fopen(log_file_path,  "a");

    fgets(line, 255, proc_stat_file);

    sscanf( line + 4,                           // skip the "cpu " part
            "%d %d %d %d %d %d %d %d %d %d",
            &statistics[iterations].user,
            &statistics[iterations].user_nice,
            &statistics[iterations].system,
            &statistics[iterations].idle,
            &statistics[iterations].iowait,
            &statistics[iterations].irq,
            &statistics[iterations].softirq,
            &statistics[iterations].steal,
            &statistics[iterations].guest,
            &statistics[iterations].guest_nice
          );
    
    iterations++;

    if(iterations >= iterations_per_interval()){
      iterations = 0;
      
      struct cpu_usage usage = interval_statistics(statistics);

      fprintf(log_file, "%.2f %.2f %.2f \n", usage.minimal, usage.average, usage.maximal);
    }

    fclose(proc_stat_file);
    fclose(log_file);

    sleep(period);
  }
  exit(0);
}
