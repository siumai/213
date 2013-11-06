#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cachelab.h"

struct line {
    int valid;
    unsigned long tag;
};

typedef struct line LINE;

int main()
{
    /*unsigned int test = 0x80000000;
    test = test >> 4;
    printf("shift %x\n", test);*/
    
    int s, E, b, sets;
    s = 1;
    E = 1;
    b = 1;
    sets = pow(2, s);
    int tagShift = b + s;
    unsigned long setIndexMask = (sets-1)<<b;

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
    unsigned long address;
    int rsize;
    int hit = 0, miss = 0, eviction = 0;

    while(4 == fscanf(ifp, " %c %lx %c %d", &instruction, &address, &cm, &rsize)) {
	printf("%c \t %lx \t %d\n", instruction, address, rsize);
	
	int setIndex = (address & setIndexMask)>>b;
	printf("set index: %x\n", setIndex);
	unsigned long tg = address >> tagShift;
	printf("tag: %lx\n", tg);
	LINE *lnp = &(cache[setIndex][0]);
	if((*lnp).valid) {
	    printf("cache tag: %lx\n", (*lnp).tag);
	}
	if(instruction=='L' || instruction=='S') {
	    if((*lnp).valid==1 && tg == (*lnp).tag) {
		hit++;
		puts("hit");
	    } else {
		miss++;
		if((*lnp).valid==1) {
		    puts("e");
		    eviction++;
		}
		(*lnp).valid = 1;
		(*lnp).tag = tg;
		puts("miss");
	    }
	} else if(instruction=='M') {
	    if((*lnp).valid==1 && tg == (*lnp).tag) {
		hit += 2;
		puts("hit*2");
	    } else {
		hit++;
		miss++;
		if((*lnp).valid == 1) {
		    puts("e");
		    eviction++;
		}
		(*lnp).valid = 1;
		(*lnp).tag = tg;
		puts("hit miss");
	    }	
	}
    }

    if(feof(ifp)) {
	puts("EOF");
    }
    fclose(ifp);

    for( int i = 0; i < s; i++) {
	free(cache[i]);
    }
    free(cache);

    printSummary(hit, miss, eviction);
    return 0;
}
