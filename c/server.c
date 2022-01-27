/* TCP/IPプログラム（サーバ）*/

#include    <stdio.h>
#include    <unistd.h>
#include    <stdlib.h>
#include    <string.h>
#include    <errno.h>
#include    <signal.h>
#include    <sys/param.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <netdb.h>
#include    <sys/wait.h>

const int p = 12345;

void ChildEnd(int sig)
{
	int     pid;

    /* 子プロセスの終了を待つ */
    pid=wait(NULL);
    fprintf(stderr,"ChildEnd:%d\n",pid);
}

int main()
{
	struct servent		*se;
	struct sockaddr_in	my;
	struct sockaddr_in	from;
	int		soc,portno,opt;
	int		acc,len;
	int		pid;
	char	buf[512], *ptr;

    /* 子プロセス終了シグナルのセット */
    signal(SIGCLD,ChildEnd);

    /* ソケットの生成 */
    if((soc=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        return(-1);
    }

    /* ソケットオプション（再利用フラグ）設定 */
    opt=1;
    if(setsockopt(soc,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(int))!=0){
        perror("setsockopt");
        return(-1);
    }

    /* ポート番号の決定 */
	memset((char *)&my,0,sizeof(my));

    my.sin_family=AF_INET;
    my.sin_port=htons(p);
    fprintf(stderr,"port=%d\n",(my.sin_port));

    /* ソケットにアドレスを指定 */
    if(bind(soc,(struct sockaddr *)&my,sizeof(my))==-1){
        perror("bind");
        return(-1);
    }

    /* アクセスバックログの指定 */
    if(listen(soc,SOMAXCONN)==-1){
        perror("listen");
        return(-1);
    }

    fprintf(stderr,"ready for accept\n");

    while(1){
        len=sizeof(from);
        /* 接続受付 */
        acc=accept(soc,(struct sockaddr *)&from,&len);
        if(acc<0){
            perror("accept");
        }
        fprintf(stderr,"accept:%s:%d\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port));
        if((pid=fork())==0){
            /* 子プロセス */
            close(soc);
            /* 送受信 */
            while(1){
                /* 受信 */
                if((len=recv(acc,buf,sizeof(buf),0))<0){
                    /* エラー */
                    perror("recv");
                    break;
                }
                if(len==0){
                    /* エンド・オブ・ファイル */
                    fprintf(stderr,"recv:EOF\n");
                    break;
                }
                /* 文字列化・表示 */
                buf[len]='\0';
                if((ptr=strpbrk(buf,"\r\n"))!=NULL){
                    *ptr='\0';
                }
                fprintf(stderr,"[client]%s\n",buf);
                /* 応答文字列作成 */
                //strcat(buf,":OK\r\n");
                strcpy(buf,":OK\r\n");
                len=strlen(buf);
                /* 応答 */
                if((len=send(acc,buf,len,0))<0){
                    /* エラー */
                    perror("send");
                    break;
                }
            }
            /* アクセプトソケットクローズ */
            close(acc);
            return(0);
        }
        fprintf(stderr,"ChildStart:%d\n",pid);
        setpgid(pid,getpid());
        close(acc);
        acc=0;
    }

    /* ソケットクローズ */
    close(soc);

    return(0);
}
