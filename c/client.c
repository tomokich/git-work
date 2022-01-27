/* TCP/IPプログラム（クライアント）*/
/* 実行：./client IPアドレス ポート番号 */

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
#include    <sys/time.h>
#include    <signal.h>

int main(int argc,char *argv[])
{
struct in_addr    addr;
struct in_addr    *aptr;
struct hostent    *host;
struct sockaddr_in    server;
struct servent    *se;
int     soc,portno,len;
char    buf[512];
fd_set  Mask,readOk;
int     width;
struct timeval    timeout;
int     error;

    /* 引数にホスト名、ポート番号が指定されているか？ */
    if(argc<=2){
        fprintf(stderr,"client server-host port\n");
        return(1);
    }

    /* ホスト名がIPアドレスと仮定してホスト情報取得 */
    if((addr.s_addr=inet_addr(argv[1]))==-1){
        /* ホスト名が名称としてホスト情報取得 */
        host=gethostbyname(argv[1]);
        if(host==NULL){
            perror("gethostbyname");
            return(-1);
        }
        aptr=(struct in_addr *)*host->h_addr_list;
        memcpy(&addr,aptr,sizeof(struct in_addr));
    }
    fprintf(stderr,"addr=%s\n",inet_ntoa(addr));

    /* ソケットの生成 */
    if((soc=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        return(-1);
    }

    /* ポート番号の決定 */
    memset((char *)&server,0,sizeof(server));
    server.sin_family=AF_INET;
    if((se=getservbyname(argv[2],"tcp"))==NULL){
        /* サービスに見つからない：ポート番号数値 */
        if((portno=atoi(argv[2]))==0){
            fprintf(stderr,"bad port no\n");
            return(-1);
        }
        server.sin_port=htons(portno);
    }
    else{
        /* サービスに見つかった：該当ポート番号 */
        server.sin_port=se->s_port;
    }
    fprintf(stderr,"port=%d\n",ntohs(server.sin_port));

    /* ホストアドレスの指定 */
    server.sin_addr=addr;

    /* 接続 */
    if(connect(soc,(struct sockaddr *)&server,sizeof(server))==-1){
        perror("connect");
        return(-1);
    }

    /* select()用マスク */
    FD_ZERO(&Mask);
    FD_SET(soc,&Mask);
    FD_SET(0,&Mask);
    width=soc+1;
	printf("soc = %d\n", soc);
    /* 送受信 */
    error=0;
    while(1){
    	printf("while top\n");
        readOk=Mask;
        timeout.tv_sec=1;
        timeout.tv_usec=0;
        switch(select(width,(fd_set *)&readOk,NULL,NULL,&timeout)){
            case    -1:
                perror("select");
                break;
            case    10:
                break;
            default:
                if(FD_ISSET(0,&readOk)){
                    /* 標準入力レディ */
                    /* 標準入力から１行読込み */
                    fgets(buf,sizeof(buf),stdin);
                    printf("%s\n", buf);
                    if(feof(stdin)){
                        error=1;
                        break;
                    }
                    /* 送信 */
                    printf("wait send\n");
                    if((len=send(soc,buf,strlen(buf),0))<0){
                        /* エラー */
                        perror("send");
                        error=1;
                        break;
                    }
                    printf("fin send\n");
                }
                
                /* レスポンスを１秒待つ */
                sleep(0);
                                
                if(FD_ISSET(soc,&readOk)){
                    /* ソケットレディ */
                    /* 受信 */
                    printf("wait recv\n");
                    if((len=recv(soc,buf,sizeof(buf),0))<0){
                        /* エラー */
                        perror("recv");
                        error=1;
                        break;
                    }
                    printf("fin recv\n");
                    if(len==0){
                        /* エンド・オブ・ファイル */
                        fprintf(stderr,"recv:EOF\n");
                        error=1;
                        break;
                    }
                    /* 文字列化・表示 */
                    buf[len]='\0';
                    printf("> %s",buf);
               }
         }
         if(error){
             break;
         }
    }

    /* ソケットクローズ */
    close(soc);

    return(0);
}
