/* TCP/IP�v���O�����i�N���C�A���g�j*/
/* ���s�F./client IP�A�h���X �|�[�g�ԍ� */

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

    /* �����Ƀz�X�g���A�|�[�g�ԍ����w�肳��Ă��邩�H */
    if(argc<=2){
        fprintf(stderr,"client server-host port\n");
        return(1);
    }

    /* �z�X�g����IP�A�h���X�Ɖ��肵�ăz�X�g���擾 */
    if((addr.s_addr=inet_addr(argv[1]))==-1){
        /* �z�X�g�������̂Ƃ��ăz�X�g���擾 */
        host=gethostbyname(argv[1]);
        if(host==NULL){
            perror("gethostbyname");
            return(-1);
        }
        aptr=(struct in_addr *)*host->h_addr_list;
        memcpy(&addr,aptr,sizeof(struct in_addr));
    }
    fprintf(stderr,"addr=%s\n",inet_ntoa(addr));

    /* �\�P�b�g�̐��� */
    if((soc=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        return(-1);
    }

    /* �|�[�g�ԍ��̌��� */
    memset((char *)&server,0,sizeof(server));
    server.sin_family=AF_INET;
    if((se=getservbyname(argv[2],"tcp"))==NULL){
        /* �T�[�r�X�Ɍ�����Ȃ��F�|�[�g�ԍ����l */
        if((portno=atoi(argv[2]))==0){
            fprintf(stderr,"bad port no\n");
            return(-1);
        }
        server.sin_port=htons(portno);
    }
    else{
        /* �T�[�r�X�Ɍ��������F�Y���|�[�g�ԍ� */
        server.sin_port=se->s_port;
    }
    fprintf(stderr,"port=%d\n",ntohs(server.sin_port));

    /* �z�X�g�A�h���X�̎w�� */
    server.sin_addr=addr;

    /* �ڑ� */
    if(connect(soc,(struct sockaddr *)&server,sizeof(server))==-1){
        perror("connect");
        return(-1);
    }

    /* select()�p�}�X�N */
    FD_ZERO(&Mask);
    FD_SET(soc,&Mask);
    FD_SET(0,&Mask);
    width=soc+1;
	printf("soc = %d\n", soc);
    /* ����M */
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
                    /* �W�����̓��f�B */
                    /* �W�����͂���P�s�Ǎ��� */
                    fgets(buf,sizeof(buf),stdin);
                    printf("%s\n", buf);
                    if(feof(stdin)){
                        error=1;
                        break;
                    }
                    /* ���M */
                    printf("wait send\n");
                    if((len=send(soc,buf,strlen(buf),0))<0){
                        /* �G���[ */
                        perror("send");
                        error=1;
                        break;
                    }
                    printf("fin send\n");
                }
                
                /* ���X�|���X���P�b�҂� */
                sleep(0);
                                
                if(FD_ISSET(soc,&readOk)){
                    /* �\�P�b�g���f�B */
                    /* ��M */
                    printf("wait recv\n");
                    if((len=recv(soc,buf,sizeof(buf),0))<0){
                        /* �G���[ */
                        perror("recv");
                        error=1;
                        break;
                    }
                    printf("fin recv\n");
                    if(len==0){
                        /* �G���h�E�I�u�E�t�@�C�� */
                        fprintf(stderr,"recv:EOF\n");
                        error=1;
                        break;
                    }
                    /* �����񉻁E�\�� */
                    buf[len]='\0';
                    printf("> %s",buf);
               }
         }
         if(error){
             break;
         }
    }

    /* �\�P�b�g�N���[�Y */
    close(soc);

    return(0);
}
