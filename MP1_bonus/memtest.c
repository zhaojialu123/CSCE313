#include <stdlib.h>
#include <stdio.h>
#include <math.h> 
#include <getopt.h>
#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) {

  // input parameters (basic block size, memory length)
	int b = 2;
	int M = 20;  // so we have space for 11 items

	// read the basic block size and the memory size from the command line
	int opt;
	while((opt = getopt(argc, argv, "b:M:")) != -1)
	{
		switch(opt)
		{
			case 'b':
			b = atoi(optarg);
			break;
			case 'M':
			M = atoi(optarg);
			break;
			default:
			b = 128;
			M = b * 8;
		}
	}

  // init_allocator(basic block size, memory length)
	init_allocator(b,M);

	//my_malloc(512-sizeof(node));

  ackerman_main();
  //printf("here\n");

  release_allocator();
}
