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
    //int mmap_size = MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE*2;
    foo = open("/dev/mem", O_RDWR | O_NONBLOCK);
    bram_ipt_matrix = static_cast<uint32_t *>(mmap(NULL, (MATRIX_SIZE*MATRIX_SIZE+MATRIX_SIZE*2)*sizeof(float), PROT_WRITE | PROT_READ, MAP_SHARED, foo, MATRIX_ADDR));
    bram_ipt_vector = bram_ipt_matrix + MATRIX_SIZE*MATRIX_SIZE;

    bram_opt_vector = bram_ipt_vector + MATRIX_SIZE;
    
    if(((int)bram_ipt_matrix == -1) || ((int)bram_ipt_vector == -1)){
        printf("error, %p %p\n", bram_ipt_matrix, bram_ipt_vector);
        printf("error code : %s\n", strerror(errno));
    }

    fpga_ip = static_cast<uint32_t *>(mmap(NULL, sizeof(uint32_t), PROT_WRITE, MAP_SHARED, foo, INSTRUCTION_ADDR));
}

void fpga_tb::fpga_load_execute_and_copy(const uint16_t *ipt_matrix_fix16, const uint16_t *ipt_vector_fix16, uint32_t *your_vector_fix32)
{
	//Copy data to BRAM(or you may use DMA).

    for(int i = 0; i < MATRIX_SIZE*MATRIX_SIZE; i++) {
        bram_ipt_matrix[i] = (uint32_t) ipt_matrix_fix16[i];
    }
    for(int i = 0; i < MATRIX_SIZE; i++)
        bram_ipt_vector[i] = (uint32_t) ipt_vector_fix16[i];

    *fpga_ip = 0x5555;
    while(*fpga_ip == 0x5555) {}
	//and copy BRAM's data to DRAM space.
/*
    for(int i = 0; i < MATRIX_SIZE/2; i++){
    uint32_t tmp = static_cast<uint32_t>(ipt_vector_fix16[i]);
        tmp = tmp << 16;
        tmp += static_cast<uint32_t>(ipt_vector_fix16[i + MATRIX_SIZE/2]);
        bram_ipt_matrix[i] = tmp;
    }
    bram_ipt_vector = bram_ipt_matrix + MATRIX_SIZE/2;
    int k = 0;
    for(int i = 0; i < MATRIX_SIZE/2; i++){
        for(int j = 0; j < MATRIX_SIZE; j++){
            uint32_t tmp = static_cast<uint32_t>(ipt_matrix_fix16[j*MATRIX_SIZE+i]);
            tmp = tmp << 16;
            tmp += ipt_matrix_fix16[j*MATRIX_SIZE+i+MATRIX_SIZE/2];
            bram_ipt_vector[k] = tmp;
            k++;
        }
    }
    bram_opt_vector = bram_ipt_vector + MATRIX_SIZE*MATRIX_SIZE/2;
*/

//    for(int i = 0; i < MATRIX_SIZE*MATRIX_SIZE; i++)
//        printf("%p : %x\n", bram_ipt_matrix+i, bram_ipt_matrix[i]);
//    for(int i = 0; i < 2*MATRIX_SIZE; i++)
//        printf("%p : %x\n", bram_ipt_vector+i, bram_ipt_vector[i]);

    for(int i = 0; i < MATRIX_SIZE; i++) {
        your_vector_fix32[i] = bram_opt_vector[i];
    }
}

void fpga_tb::fpga_cleanup()
{
    munmap(bram_ipt_matrix, (MATRIX_SIZE*MATRIX_SIZE+MATRIX_SIZE)*sizeof(uint32_t));
    munmap((void*)fpga_ip, sizeof(float));
    close(foo);
	//Cleanup allocated resources.
}
