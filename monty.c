#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* reset_doors(char* byob);
unsigned long uniform_random(unsigned long size);

char* reset_doors(char* byob)
{       
        // Set the "good" door, use lots of entropy while doing it
        unsigned good = (unsigned)uniform_random(3);
        
        // setup the doors for the contestant to choose from
        byob[good] = 1;
        byob[(good+1) % 3] = 0;
        byob[(good+2) % 3] = 0;
        
        return byob;
}

/* Modulo bias free, uniformly distributed random numbers from 0 to range-1.

Modulo bias occurs when the total number of possible random numbers is not
divisible by the desired range. In order to fix this, we will calculate a new
maximum random number and reject any higher than or equal to that. Once we have
a suitable number, we will then calculate the modulo. */

unsigned long uniform_random(unsigned long range) {
        
        // The proof is left as an exercise for the reader
        unsigned long new_max = RAND_MAX - RAND_MAX % range;
        unsigned long rand_l = (unsigned)random();

        while(rand_l >= new_max)
                rand_l = (unsigned)random();

        return rand_l % range;
}

int main(int argc, char** argv)
{
        // if wrong number of command line arguments
        if(argc != 2) {
                fprintf(stderr, "Monty hall problem simulator v1.0\nUsage %s <number of runs>\n", argv[0]);
                return 1;
        }
        
        // make an end pointer so we can check if strtol actually found a long
        char** end = malloc(sizeof(char*));
        
        long RUNS = strtol(argv[1], end, 0);
        
        /* if strtol was unable to find a number to convert, *end will point to
        the same string as argv[1] */
        
        if(strcmp(argv[1], *end) == 0) {
                fprintf(stderr, "argument must be a positive integer\n");
                return 2;
        }
        
        // Finally if somebody gives us a negative number or zero
        if(RUNS <= 0) {
                fprintf(stderr, "argument must be a positive integer\n");
                return 3;
        }
        
        // tells random() to use the /dev/urandom device for seeding
        srandomdev();
        
        // Allocate the set of doors, the win flag and the win tally
        char* doors = malloc((size_t)3);
        char win;
        int win_tally = 0;
        
        // Welcome to Let's Make a Deal
        for(int i = 0; i < RUNS; i++) {
                // reset the game
                win = 0;
                reset_doors(doors);
                
                // contestant chooses a door
                unsigned guess = (unsigned)uniform_random(3);
                
                // doors set to -1 are not considered for a win by the
                // contestant. Since this guess will be thrown away we will set
                // the door to -1
                doors[guess] = -1;
                
                
                /* FIXME: Currently, this prefers low indexed doors, should be
                random */
                // Let me reveal a bad door for you
                if(doors[0] == 0) {
                        doors[0] = -1; // door is "revealed"
                } else if(doors[1] == 0) {
                        doors[1] = -1;
                } else if(doors[2] == 0){
                        doors[2] = -1;
                }
                
                // Did you win? If you won, the array will contain two -1's and 
                // a 1. If you lost, it will contain two -1's and a 0. Simply
                // checking for a 1 is sufficient to determine a win
                if(doors[0] == 1)
                        win = 1;
                if(doors[1] == 1)
                        win = 1;
                if(doors[2] == 1)
                        win = 1;
                if(win)
                        win_tally++;
        }
        
        float percent = (float)win_tally/RUNS*100;
        
        printf("Out of %ld runs, %d rounds(%0.1f %%) were won.\n", RUNS, win_tally, percent);
}
