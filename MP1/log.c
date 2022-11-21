#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	char s[1000]; scanf("%s", s);
    DIR * dir;
    struct dirent * ptr;
    dir = opendir(s);
	char dname[1001][512]; 
	int num = 0, exist_id = 0;
    while((ptr = readdir(dir)) != NULL) {
		if( (strcmp(ptr -> d_name,"." ) == 0) || (strcmp(ptr -> d_name, "..") == 0))
			continue;
		if( (strcmp(ptr -> d_name,".loser_record" ) == 0))
			exist_id = num;
		strcpy(dname[num], ptr -> d_name);
		printf("%s\n", dname[num]);
		num++;
    }
    closedir(dir);
	strcat(s, "/");
	printf("%s\n", s);
	int dirlen = strlen(s);
	for(int i = 0; i < num; i++) {
		int newlen = dirlen + strlen(dname[i]);
		char* dname2;
		dname2 = (char*)malloc(sizeof(char)*(newlen+5));
		strcpy(dname2, s);
		strcat(dname2, dname[i]);
		strcpy(dname[i], dname2);
		printf("%s\n", dname[i]);
		free(dname2);
	}
}/*
void get_all_file(DIR* dir, char dname[1001][256]) {
	int num = 0;
	while((ptr = readdir(dir)) != NULL) {
		struct dirent * ptr;
		strcpy(dname[num], ptr -> d_name);
		printf("%s\n", dname[num]);
	num++;
    }
}
/*
void get_all_file(char s[1000], char dname[1001][256], int *num) {
	*num = 0;
	struct dirent * ptr;
	DIR * dir = opendir(s);
	while((ptr = readdir(dir)) != NULL) {
		struct dirent * ptr;
		strcpy(dname[*num], ptr -> d_name);
		num++;
    }
}
执行：
d_name : .
d_name : ..
d_name : init.d
d_name : rc0.d
d_name : rc1.d
d_name : rc2.d
d_name : rc3.d
d_name : rc4.d
d_name : rc5.d
d_name : rc6.d
d_name : rc
d_name : rc.local
d_name : rc.sysinit
struct dirent
{
    ino_t d_ino; //d_ino 此目录进入点的inode
    ff_t d_off; //d_off 目录文件开头至此目录进入点的位移
    signed short int d_reclen; //d_reclen _name 的长度, 不包含NULL 字符
    unsigned char d_type; //d_type d_name 所指的文件类型 d_name 文件名
    har d_name[256];
};*/