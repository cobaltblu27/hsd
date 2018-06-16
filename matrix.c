#include<stdio.h>

int main(){
    float matrix[64][64];
    float vector[64];
    float answer[64];
    FILE *f = fopen("./input.txt", "r");
    for(int i = 0; i < 64; i++)
        for(int j = 0; j < 64; j++)
            fscanf(f, "%x", (unsigned int*) &(matrix[i][j]));

    for(int i = 0; i < 64;i++){
        fscanf(f, "%x",(unsigned int*) &(vector[i]));
        answer[i] = 0;
    }

    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j ++){
            answer[j] += matrix[j][i] * vector[i];
            printf("+= %f * %f => %f\n",matrix[j][i],vector[i],  answer[j]);
        }
        printf("%f\n", (answer[i]));
    }
    printf("final result:\n");
    for(int i = 0; i < 64; i++)
        printf("%f\n", answer[i]);

}
