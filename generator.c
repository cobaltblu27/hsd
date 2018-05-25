#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

#define INPUT_SIZE (64 * 64 + 64)

int main(int argc, char **argv)
{
	float input[INPUT_SIZE];
	float result = 0.0f;
	int hex = 0;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	if(argc == 2 && !strcmp(argv[1], "-h")){
		hex = 1;	
	}	

	for(int i = 0 ; i < INPUT_SIZE ; i++)
	{
		do
		{
			input[i] = ((float)(rand() % 1000)) / 100.0f;
		}
		while(input[i] < 0.3f);
		if(!hex)
			printf("%f\n", input[i]);
		else
			printf("%x\n", *(unsigned int*)(&(input[i])));
	}

	return 0;

}
