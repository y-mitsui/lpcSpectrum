#include <stdio.h>
#include <stdlib.h>

void fatalError(char *message){
	fprintf(stderr,"%s",message);
	exit(EXIT_FAILURE);
}
