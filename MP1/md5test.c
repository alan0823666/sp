#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
 
int main(int argc, char *argv[])
{
	MD5_CTX md5;
	MD5Init(&md5);         
	int i;
	unsigned char* encrypt;
	unsigned char decrypt[16];
	
	FILE *fp = fopen(argv[1], "rb");
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	printf("%d\n", len);
	fseek(fp, 0, SEEK_SET);
	encrypt = (unsigned char*)malloc(len*(sizeof(unsigned char)));
	
	fread(encrypt, sizeof(unsigned char), len, fp);
	
//	printf("%s\n",encrypt);
	
	MD5Update(&md5,encrypt,strlen((char *)encrypt));
	MD5Final(&md5,decrypt);        
	
	for(i=0;i<16;i++) {
		printf("%02x",decrypt[i]);
	}
 
	getchar();
 
	return 0;
}