#include <stdio.h>

int main(int argc, char* argv[]){
  if(argc == 2){
    printf("%s\n", argv[1]);
  }
  else{
    printf("usage: eho [str], where [str] is a string of characters\n");
  }
}
