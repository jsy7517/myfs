#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void get_single(char *, int , int);
int bit_print(char *, int);

int main(){
    char data[128]={0};
    int n = 5, num = 1023;
    get_single(data, n, num);
    for(int i = 0; i< 50; i++){
        printf("%d ",bit_print(data, i)?1:0);
        
        if(i%8==7&&i!=0)
       // if(i%10==9&&i!=0)
            printf("\n");
    }
    printf("\n");
}

void get_single(char * a, int n, int num){
    // n은 data 인덱스 , 5로 나눴을때 나머지가 4인수들 못씀
    if(n%5 == 0){
        for(int i = 0; i < 8; i++)
            if(num&1<<(i+2))
                a[n]|=1<<i;
        for(int i = 6; i < 8; i++)
            if(num&1<<(7-i))
                a[n+1] |= 1<<(13-i);

    }
    else if(n%5 == 1){
        for(int i=0; i<6; i++)
            if(num&1<<(i+3))
                a[n]|= 1<<i;
        for(int i= 4; i<8; i++)
            if(num&1<<(i-4))
                a[n+1]|= 1<<i;
    }
    else if(n%5 == 2){
        for(int i= 0; i<4; i++)
            if(num&1<<(i+3))
                a[n]|= 1<<i;
        for(int i=2; i<8; i++)
            if(num&1<<(i-2))
                a[n+1]|= 1<<i;
    }
    else if(n%5 == 3){
        for(int i=0;i<2;i++)
            if(num&1<<(i+8))
                a[n]|= 1<<i;
        for(int i = 0; i <8; i++)
                if(num&1<<(i))
                    a[n+1]|= 1<<i;
    }
}
int bit_print(char * a, int n){
    char * c = (char *) a;
    int bit = c[n/8]&1<<(7-(n%8));
    return bit;
}