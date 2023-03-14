#include "kernel/types.h"
#include "user/user.h"

#define R 0
#define W 1

const uint INT_LEN = sizeof(int);

int lpipe_first_data(int lpipe[2],int *dst)
{
    if(read(lpipe[R],dst,INT_LEN)==INT_LEN){
        printf("prime %d\n",*dst);
        return 0;
    }
    return -1;
}

void transmit_data(int lpipe[2],int rpipe[2],int first)
{
    int data;
    while(read(lpipe[R],&data,INT_LEN)==INT_LEN){
        if(data%first){
            write(rpipe[W],&data,INT_LEN);
        }
    }
    close(lpipe[R]);
    close(rpipe[W]);
    return;
}

void primes(int lpipe[2]){
    close(lpipe[W]);
    int first;
    if(lpipe_first_data(lpipe,&first)==0){
        int p[2];
        pipe(p);
        transmit_data(lpipe,p,first);

        if(fork()==0){
            primes(p);
        }
        else{
            close(p[R]);
            wait(0);
        }
    }
    return;
}

int main(){
    int p[2];
    pipe(p);

    for(int i=2;i<=35;i++){
        write(p[W],&i,INT_LEN);
    }

    if(fork()==0){
        primes(p);
    }
    else{
        close(p[W]);
        close(p[R]);
        wait(0);
    }
    exit(0);
}