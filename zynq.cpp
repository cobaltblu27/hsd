#include "zynq.h"

#include <cstring>
#include <sys/time.h>

#include <cstdio>	//for perror
#include <sys/mman.h>	//mmap

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BRAM_BASE 0x40000000

#define MATRIX_ADDR BRAM_BASE
#define IPT_VECTOR_ADDR (MATRIX_ADDR + (MATRIX_SIZE * MATRIX_SIZE * sizeof(uint32_t)))
#define OPT_VECTOR_ADDR (IPT_VECTOR_ADDR + MATRIX_SIZE * sizeof(uint32_t))

#define INSTRUCTION_ADDR 0x43C00000
#define MAGIC_CODE 0x5555

static inline float f16_to_f32(const uint32_t *input);

double fpga_calculate(uint32_t *ipt_matrix_f16, uint32_t *ipt_vector_f16, float *your_vector_f32)
{
	//Map BRAM to virtual memory space and copy data.
	int foo = open("/dev/mem", O_RDWR | O_NONBLOCK);
	
	// size of bram : 64 * 64(ipt_matrix_f16) + 64(pt_vector_f16) + 64(your_vector_f32)
	int *fpga_bram = (int *) mmap(NULL, (MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE + MATRIX_SIZE) * sizeof(uint32_t) , PROT_WRITE, MAP_SHARED, foo, BRAM_BASE);
	int *fpga_ip = (int *) mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, foo, INSTRUCTION_ADDR);
	
	//copy input matrix to bram
	for(int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++)
		*(fpga_bram + i) = ipt_matrix_f16[i];
	//copy input vector to bram
	for(int i = 0; i < MATRIX_SIZE; i++)
		*(fpga_bram + i + MATRIX_SIZE * MATRIX_SIZE) = ipt_vector_f16[i];
	 	
    *(fpga_ip) = MAGIC_CODE;
	while(*fpga_ip == MAGIC_CODE);

	struct timeval start, end;
	gettimeofday(&start, NULL);

	//Run IP and copy value to DRAM space
	
	gettimeofday(&end, NULL);
	
	//Cleanup allocated resources (optional).

	return (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec)) / 1000000.0;
}


double arm_calculate(uint32_t *ipt_matrix_f16, uint32_t *ipt_vector_f16, float *arm_vector_f32)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);

	memset(arm_vector_f32, 0, sizeof(float) << SIZE_SHIFTER);

	for(size_t i = -1 ; ++i < matrix_size * matrix_size ; arm_vector_f32[i >> SIZE_SHIFTER]
		+= f16_to_f32(ipt_matrix_f16 + i) * f16_to_f32(ipt_vector_f16 + (i & ((1 << SIZE_SHIFTER) - 1))));

	gettimeofday(&end, NULL);
	return (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec)) / 1000000.0;
}

static inline float f16_to_f32(const uint32_t *input)
{
	char *iptc = (char*)input + 2;

	uint32_t half_precision = 0;
	memcpy(&half_precision, iptc + 1, 1);
	memcpy(((void*)&half_precision) + 1, iptc, 1);

	uint32_t opt 	= ((half_precision & 0x8000) << 16)
			| ((((half_precision & 0x7C00) >> 10) + 112) << 23)
			| ((half_precision & 0x3FF) << 13);

	float casted_opt;
	memcpy(&casted_opt, &opt, sizeof(float));
	return casted_opt;
}
