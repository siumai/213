#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cachelab.h"

struct line {
    int valid;
    unsigned long tag;
    unsigned long lru;
};

typedef struct line LINE;

int main()
{    
    int s, E, b, sets;
    s = 5;
    E = 1;
    b = 5;
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
	    cache[i][j].lru = 0;
	}
    }

    FILE *ifp;
    char filename[] = "./traces/trans.trace";
    ifp = fopen(filename, "r");
    if(ifp == NULL) {
        fprintf(stderr, "Can't open input file\n");
	return 1;
    }

    char instruction, cm;
    unsigned long address;
    int rsize;
    int hit = 0, miss = 0, eviction = 0, idx = 1;

    while(4 == fscanf(ifp, " %c %lx %c %d", &instruction, &address, &cm, &rsize)) {
	//printf("%c \t %lx \t %d\n", instruction, address, rsize);
	printf("%c \t %lx \t %d ", instruction, address, rsize);
	if(instruction=='I') {
	    printf("\n");
	    continue;
	}
	
	int setIndex = (address & setIndexMask)>>b;
	printf("set %x ", setIndex);
	unsigned long tg = address >> tagShift;
	printf("tag %lx ", tg);
	
	int wasHit = 0;
	//int evicE = 0;
	//int leastIdx = 0;
	int isFull = 1;

	for( int e = 0; e < E; e++) {
	    
	    LINE *lnp = &(cache[setIndex][e]);
	    if((*lnp).valid==1 && tg == (*lnp).tag) {
		if(instruction=='L' || instruction=='S') {
		    hit++;
		    printf("hit ");
		} else if(instruction=='M') {
		    hit+=2;
		    printf("hit hit ");
		}
		wasHit = 1;
		(*lnp).lru = idx;
		//break;
	    } else {
		if((*lnp).valid==0) {
		    isFull=0;
		}
		/*if((*lnp).lru < leastIdx) {
		    leastIdx = (*lnp).lru;
		    evicE = e;
		}*/
	    }
	}

	if(wasHit==0) {
	    if(instruction=='L' || instruction=='S') {
		miss++;
		printf("miss ");
	    } else if(instruction=='M') {
		hit++;
		miss++;
		printf("miss hit ");
	    }

	    if(isFull==0) {
		for( int e = 0; e < E; e++) {
	    	    LINE *lnp = &(cache[setIndex][e]);
	    	    if((*lnp).valid==0) {
			(*lnp).valid = 1;
			(*lnp).tag = tg;
			(*lnp).lru = idx;
			printf("not full, take line %d ", e);
			break;
		    }
		}
	    }
	    
	    if(isFull==1) {
		eviction++;
		printf("eviction ");
		int targetE = 0;
		int leastInd = cache[setIndex][0].lru;
		for( int e = 0; e < E; e++) {
		    if(cache[setIndex][e].lru < leastInd) {
			leastInd = cache[setIndex][e].lru;
			targetE = e;
		    }
		}
		printf("takes line %d ", targetE);
		cache[setIndex][targetE].valid = 1;
	    	cache[setIndex][targetE].tag = tg;
	    	cache[setIndex][targetE].lru = idx;
	    }
	    /*printf("takes line %d ", evicE);
	    cache[setIndex][evicE].valid = 1;
	    cache[setIndex][evicE].tag = tg;
	    cache[setIndex][evicE].lru = idx;*/
	}
	printf("\n");
	idx++;
	
	/*LINE *lnp = &(cache[setIndex][0]);
	
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
	}*/
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
