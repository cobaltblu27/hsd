#include<stdio.h>

int main(){
    FILE *f = fopen("input.txt", "r");
    int h;
    while(fscanf(f, "%x", &h) != EOF)
        printf("%d\n", h);
    return 0;
}
