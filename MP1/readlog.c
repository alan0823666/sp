#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
struct File_info{
	uint8_t namesize;
	char filename[256];
};
typedef struct File_info file_info;
struct Info{
	//uint32_t commit_n, file_n, add_n, modify_n, copy_n, del_n, commit_size;
	uint32_t num[7];
	file_info *files;
};
typedef struct Info info;
uint32_t transfrom(int offset, unsigned char* buf) {
	uint32_t ret = 0;
	for(int i = 0; i < 4; i++) {
		uint32_t b = (uint32_t)buf[i+offset];
		ret |= b >> 8*i;
	}
	return ret;	
}
int main(int argc, char *argv[]) {
	char s[1000];
	scanf("%s", &s);
	FILE *fp = fopen(s, "rb");

	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	printf("%d\n", len);
	fseek(fp, 0, SEEK_SET);
	unsigned char *buffer = (unsigned char*)malloc(len*(sizeof(unsigned char)));
	fread(buffer,1, len, fp);
	int offset = 0;
	info commit_log;
	for(int i = 0; i < 7; i++) {
		uint32_t a = transfrom(offset, buffer);
		offset += 4;
		printf("%u ", a);
		commit_log.num[i] = a;
	}
	file_info files[commit_log.num[1]];
	// add
	for(int i = 0; i < commit_log.num[1]; i++) {
		uint8_t name_size = (uint8_t)buffer[offset];
		printf("%u ", name_size);
		offset++;
		files[i].namesize = name_size;
		for(int k = 0; k < name_size; k++) {
			files[i].filename[k] = buffer[offset];
			offset++;
		}
		files[i].filename[name_size] = '\0';
		printf("%s\n", files[i].filename);
	}/*
	for(int i = 0; i < commit_log.num[1]; i++) {
		for(int i = 0; i < 16; i++) {
			printf("%02x", buffer[offset]);
			offset++;
		}
		printf("\n");
	}*/
	
}