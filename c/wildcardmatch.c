/* ワイルドカードマッチ */
#include	<stdio.h>
#include	<string.h>

int LikeCheck(char *str_in, char *check_in)
{
	while(1){
		switch(*check_in){
			case '\0':
				if(*str_in == '\0'){
					return(1);
				}else{
					return(0);
				}
			case '*':
				check_in++;
				while(1){
					if(LikeCheck(str_in,check_in)){
						return(1);
					}
					str_in++;
					if(*str_in == '\0'){
						return(0);
					}
				}
			case '?':
				if(*str_in == '\0'){
					return(0);
				}
				break;
			default:
				if(*str_in != *check_in){
					return(0);
				}
				break;
		}	
		str_in++;
		check_in++;
	}
}

int main()
{
	char str[512], check[512];
	char *ptr;
	int ret;
	
	fprintf(stderr, "check=");
	fgets(check, sizeof(check), stdin);
	if((ptr=strrchr(check, '\n')) != NULL){
		*ptr='\0';
	}
	while(1){
		fprintf(stderr, "str=");
		fgets(str, sizeof(str), stdin);
		if((ptr=strrchr(str,'\n'))!=NULL){
			*ptr='\0';
		}
		ret=LikeCheck(str,check);
		printf("LikeCheck(%s,%s)=%d\n",str,check,ret);
	}
	return(0);
}
