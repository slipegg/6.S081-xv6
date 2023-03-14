#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{

    int p[2];
    pipe(p);
    char buf[512];
    if(fork()==0){
        int pid=getpid();
        read(p[0],buf,sizeof(buf));
        printf("%d: received %s\n",pid,buf);
        write(p[1],"pong",4);
        close(p[1]);
        close(p[0]);
    }else{
        int pid=getpid();
        write(p[1],"ping",4);
        read(p[0],buf,sizeof(buf));
        printf("%d: received %s\n",pid,buf);
        close(p[1]);
        close(p[0]);
    }
    exit(0);
}
