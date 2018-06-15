#include<stdio.h>

float fixtoflt(int a);

int main(){
    float matrix[64][64];
    float vector[64];
    float answer[64];
    FILE *f = fopen("./input.txt", "r");
    int tmp;
    for(int i = 0; i < 64; i++)
        for(int j = 0; j < 64; j++){
            int tmp;
            fscanf(f, "%x", &tmp);
            matrix[i][j] = fixtoflt(tmp);
        }

    for(int i = 0; i < 64;i++){
        fscanf(f, "%x", &tmp);
        vector[i] = fixtoflt(tmp);
        answer[i] = 0;
    }

    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 64; j ++){
            answer[j] += matrix[j][i]*vector[i];
            printf("+= %f * %f => %f\n", matrix[j][i], vector[i], answer[j]);
        }
        printf("%f\n", (answer[i]));
    }
    printf("final result:\n");
    for(int i = 0; i < 64; i++)
        printf("%f\n", answer[i]);

}

float fixtoflt(int a){
    float res = 0.0;
    float half = 0.5;
    res += a >> 8;
    for(int i = 0; i < 8; i++){
        int frac_i = ~(1 << i);
        if(frac_i != 0){
            float frac = frac_i;
            for(int j = 8; j >0; j--)
                frac *= half;
            res += frac;
        }
    }
    return res;
}
