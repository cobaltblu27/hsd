#include<stdio.h>

float fixtoflt(int a);

int main(){
    unsigned int matrix[64][64];
    unsigned int vector[64];
    unsigned int answer[64];
    FILE *f = fopen("./input.txt", "r");
    for(int i = 0; i < 64; i++)
        for(int j = 0; j < 64; j++){
            fscanf(f, "%x", &(matrix[i][j]));
        }

    for(int i = 0; i < 64;i++){
        fscanf(f, "%x", &(vector[i]));
        answer[i] = 0;
    }

    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j ++){
            answer[j] += (matrix[j][i]*vector[i]) >> 8;
            printf("+= %x * %x => %d\n", matrix[j][i], vector[i], answer[j]>>8);
        }
        printf("%dth result: %x\n", i,(answer[i]));
    }
    printf("final result:\n");
    for(int i = 0; i < 64; i++)
        printf("%d\n", answer[i]>>8);

}
