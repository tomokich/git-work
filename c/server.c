/* TCP/IP�v���O�����i�T�[�o�j*/

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

    /* �q�v���Z�X�̏I����҂� */
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

    /* �q�v���Z�X�I���V�O�i���̃Z�b�g */
    signal(SIGCLD,ChildEnd);

    /* �\�P�b�g�̐��� */
    if((soc=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        return(-1);
    }

    /* �\�P�b�g�I�v�V�����i�ė��p�t���O�j�ݒ� */
    opt=1;
    if(setsockopt(soc,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(int))!=0){
        perror("setsockopt");
        return(-1);
    }

    /* �|�[�g�ԍ��̌��� */
	memset((char *)&my,0,sizeof(my));

    my.sin_family=AF_INET;
    my.sin_port=htons(p);
    fprintf(stderr,"port=%d\n",(my.sin_port));

    /* �\�P�b�g�ɃA�h���X���w�� */
    if(bind(soc,(struct sockaddr *)&my,sizeof(my))==-1){
        perror("bind");
        return(-1);
    }

    /* �A�N�Z�X�o�b�N���O�̎w�� */
    if(listen(soc,SOMAXCONN)==-1){
        perror("listen");
        return(-1);
    }

    fprintf(stderr,"ready for accept\n");

    while(1){
        len=sizeof(from);
        /* �ڑ���t */
        acc=accept(soc,(struct sockaddr *)&from,&len);
        if(acc<0){
            perror("accept");
        }
        fprintf(stderr,"accept:%s:%d\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port));
        if((pid=fork())==0){
            /* �q�v���Z�X */
            close(soc);
            /* ����M */
            while(1){
                /* ��M */
                if((len=recv(acc,buf,sizeof(buf),0))<0){
                    /* �G���[ */
                    perror("recv");
                    break;
                }
                if(len==0){
                    /* �G���h�E�I�u�E�t�@�C�� */
                    fprintf(stderr,"recv:EOF\n");
                    break;
                }
                /* �����񉻁E�\�� */
                buf[len]='\0';
                if((ptr=strpbrk(buf,"\r\n"))!=NULL){
                    *ptr='\0';
                }
                fprintf(stderr,"[client]%s\n",buf);
                /* ����������쐬 */
                //strcat(buf,":OK\r\n");
                strcpy(buf,":OK\r\n");
                len=strlen(buf);
                /* ���� */
                if((len=send(acc,buf,len,0))<0){
                    /* �G���[ */
                    perror("send");
                    break;
                }
            }
            /* �A�N�Z�v�g�\�P�b�g�N���[�Y */
            close(acc);
            return(0);
        }
        fprintf(stderr,"ChildStart:%d\n",pid);
        setpgid(pid,getpid());
        close(acc);
        acc=0;
    }

    /* �\�P�b�g�N���[�Y */
    close(soc);

    return(0);
}
