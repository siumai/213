#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cachelab.h"

struct line {
    int valid;
    long tag;
};

typedef struct line LINE;

int main()
{
    //printf("%lu", sizeof(unsigned long long));
    
    int s, E, sets;
    s = 1;
    E = 1;
    sets = pow(2, s);

    LINE** cache;
    cache = (LINE**) malloc(sets*sizeof(LINE*));
    for( int i = 0; i < sets; i++) {
	cache[i] = (LINE*) malloc(E*sizeof(LINE));
    }

    for( int i = 0; i < sets; i++) {
	for( int j = 0; j < E; j++) {
	    cache[i][j].valid = 0;
	}
    }

    FILE *ifp;
    char filename[] = "./traces/yi2.trace";
    ifp = fopen(filename, "r");
    if(ifp == NULL) {
        fprintf(stderr, "Can't open input file\n");
	return 1;
    }

    char instruction, cm;
    long address;
    int rsize;
    while(4 == fscanf(ifp, " %c %lx %c %d", &instruction, &address, &cm, &rsize)) {
	printf("%c \t %lx \t %d\n", instruction, address, rsize);
    }

    if(feof(ifp)) {
	puts("EOF");
    }
    fclose(ifp);

    for( int i = 0; i < s; i++) {
	free(cache[i]);
    }
    free(cache);

    printSummary(0, 0, 0);
    return 0;
}
