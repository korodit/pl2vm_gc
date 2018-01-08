#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("FATAL ERROR:You must specify an input file\n");
    }
    printf("argc: %d,input file: %s \n",argc,argv[1]);
}