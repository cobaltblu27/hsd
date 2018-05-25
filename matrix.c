#include<stdio.h>

int main(){
	float matrix[64][64];
	float vector[64];
	float answer[64];
	FILE *f = fopen("./input.txt", "r");
	for(int i = 0; i < 64; i++){
		for(int j = 0; j < 64; j++){
			fscanf(f, "%f", &(matrix[i][j]));
		}
		answer[i] = 0.0f;
	}
	for(int i = 0; i < 64;i++)
		fscanf(f, "%f", &(vector[i]));
	for(int i = 0; i < 64; i++){
		for(int j = 0; j < 64; j ++){
			answer[j] += matrix[j][i] * vector[i];
			printf("+= %f * %f => %f\n",matrix[j][i],vector[i],  answer[j]);
		}
		printf("%f\n", (answer[i]));
	}
	
}
