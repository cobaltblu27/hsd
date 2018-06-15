#include "fpga.h"

#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/mman.h>

#define BRAM_BASE 0x40000000

#define MATRIX_ADDR BRAM_BASE
#define IPT_VECTOR_ADDR (MATRIX_ADDR + (MATRIX_SIZE * MATRIX_SIZE * sizeof(uint16_t)))
#define OPT_VECTOR_ADDR (IPT_VECTOR_ADDR + MATRIX_SIZE * sizeof(uint16_t))

#define INSTRUCTION_ADDR 0x43C00000
#define MAGIC_CODE 0x5555

//you can add variables in here too (It is recommanded to modify fpga_tb class's private field).

void fpga_tb::fpga_allocate_resources()
{
	//allocate resource.
}

void fpga_tb::fpga_load_execute_and_copy(const uint16_t *ipt_matrix_fix16, const uint16_t *ipt_vector_fix16, uint32_t *your_vector_fix32)
{
    int foo = open("/dev/mem", O_RDWR | O_NONBLOCK);

    int *fpga_bram = (int *) mmap(NULL, (MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE + MATRIX_SIZE)*sizeof(uint32_t) , PROT_WRITE, MAP_SHARED, foo, BRAM_BASE);
    int *fpga_ip = (int *) mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, foo, BRAM_BASE);

    int *fpga_vector = fpga_bram + MATRIX_SIZE * MATRIX_SIZE;
    int *result_vector = fpga_vector + MATRIX_SIZE;

	//Copy data to BRAM(or you may use DMA).


	//Execute your magic code.

    *fpga_ip = MAGIC_CODE
    while(*fpga_ip == MAGIC_CODE);

	//and copy BRAM's data to DRAM space.
    memcpy(your_vector_fix32, result_vector, MATRIX_SIZE);
}

void fpga_tb::fpga_cleanup()
{
	//Cleanup allocated resources.
}
