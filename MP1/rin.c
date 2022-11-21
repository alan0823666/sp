#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "md5.h"
struct cp{
	uint32_t add, mod, copy, del;
	int add_id[1000], mod_id[1000], copy_id[1000][2], del_id[1000];
};
typedef struct cp stat;
struct MD5{
	uint8_t namesize;
	char filename[256];
	unsigned char hash[16];
};
typedef struct MD5 md5;
struct File_info{
	uint8_t namesize;
	char filename[256];
};

typedef struct File_info file_info;

struct Info{
	//uint32_t commit_n, file_n, add_n, modify_n, copy_n, del_n, commit_size;
	uint32_t num[7];
	file_info *add, *mod,*becopy, *copy, *del;
	md5 *file_md5;
};
typedef struct Info info;
int same_hash(unsigned char hash1[16], unsigned char hash2[16]) {
	for(int i = 0; i < 16; i++)
		if(hash1[i] != hash2[i])
			return 0;
	return 1;
}
void get_md5(unsigned char* encrypt, unsigned char decrypt[16]) {
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5,encrypt,strlen((char *)encrypt));
	MD5Final(&md5,decrypt);    
}
stat md5_cmp(md5* ori, md5* now, int n1, int n2) {
	stat ret;
	ret.add = ret.mod = ret.copy = ret.del = 0;
	/*for(int i = 0; i < 1000; i++) {
		ret.add_id[i] = ret.mod_id[i] = ret.copy_id[i][0]  = ret.copy_id[i][1] = ret.del_id[i] = -1;
	}*/
	for(int i = 0; i < n2; i++) {
		int name = -1, hashing = -1;
		if(strcmp(now[i].filename, ".loser_record") == 0)
			continue;
		for(int k = 0; k < n1; k++) {
			if(strcmp(ori[k].filename, now[i].filename) == 0) {
				name = k;
			}
			if(same_hash(ori[k].hash, now[i].hash)) {
				if(hashing == -1)
					hashing = k;
			}
		}
		if(name == -1 && hashing == -1) {
			ret.add_id[ret.add] = i;
			ret.add++;
		}
		else if(name != -1 && hashing != -1)
			continue;
		else if(name != -1 && hashing == -1) {
			ret.mod_id[ret.mod] = name;
			ret.mod++;
		}
		else if(name == -1 && hashing != -1) {
			ret.copy_id[ret.copy][0] = hashing;
			ret.copy_id[ret.copy][1] = i;
			ret.copy++;
		}
	}
	for(int i = 0; i < n1; i++) {
		int flag = 0;
		for(int k = 0; k < n2; k++)
			if(strcmp(ori[i].filename, now[k].filename) == 0) {
				flag = 1;
				break;
			}
		if(flag == 0) {
			ret.del_id[ret.del] = i;
			ret.del++;
		}
	}
	return ret;
}
int cstring_cmp(const void *a, const void *b) { 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
} 
void print_info(info x) {
	printf("# commit %d\n", x.num[0]);
	printf("[new_file]\n");
	for(int i = 0; i < x.num[2]; i++)
		printf("%s\n", x.add[i].filename);
	
	printf("[modified]\n");
	for(int i = 0; i < x.num[3]; i++)
		printf("%s\n", x.mod[i].filename);
	
	printf("[copied]\n");
	for(int i = 0; i < x.num[4]; i++)
		printf("%s => %s\n", x.becopy[i].filename, x.copy[i].filename);
	
	printf("[deleted]\n");
	for(int i = 0; i < x.num[5]; i++)
		printf("%s\n", x.del[i].filename);
	
	printf("(MD5)\n");
	for(int i = 0; i < x.num[1]; i++) {
		printf("%s ", x.file_md5[i].filename);
		for(int k = 0; k < 16; k++)
			printf("%02x", x.file_md5[i].hash[k]);
		printf("\n");
	}
	
}
int main(int argc, char *argv[]) {
	DIR * dir;
    struct dirent * ptr;
    dir = opendir(argv[argc - 1]);
	char *dname[1001], *dname2[1001]; 
	uint32_t dir_num = 0; 
	int log_exist_id = -1;
    while((ptr = readdir(dir)) != NULL) {
		dname[dir_num] = (char*)malloc(sizeof(char)*512);
		if( (strcmp(ptr -> d_name,"." ) == 0) || (strcmp(ptr -> d_name, "..") == 0))
			continue;
		if( (strcmp(ptr -> d_name,".loser_record" ) == 0))
			log_exist_id = dir_num;
		strcpy(dname[dir_num], ptr -> d_name);
		//printf("%s\n", dname[dir_num]);
		dir_num++;
    }
	qsort(dname, dir_num, sizeof(char *), cstring_cmp);
	
	for(int i = 0; i < dir_num; i++)
		if(strcmp(dname[i], ".loser_record") == 0)
			log_exist_id = i;

    closedir(dir);
	strcat(argv[argc - 1], "/");
	//printf("%s\n", argv[argc - 1]);
	int dirlen = strlen(argv[argc-1]);
	for(int i = 0; i < dir_num; i++) {
		int newlen = dirlen + strlen(dname[i]);
		dname2[i] = (char*)malloc(sizeof(char)*(newlen+5));
		strcpy(dname2[i], argv[argc - 1]);
		strcat(dname2[i], dname[i]);
	}
	uint32_t commit_num = 0;
	int record_size = 1000;
	info* record = (info*)malloc(sizeof(info)*record_size);
	if(log_exist_id != -1) {
		FILE *fp = fopen(dname2[log_exist_id], "rb");
		fseek(fp, 0, SEEK_END);
		size_t len = ftell(fp);
		rewind(fp);
		while(1) {
			if(ftell(fp) == len)
				break;
			for(int i = 0; i < 7; i++) {			
				fread(&record[commit_num].num[i], sizeof(uint32_t), 1, fp);
			}
			record[commit_num].add = (file_info*)malloc(sizeof(file_info)*record[commit_num].num[2]);
			record[commit_num].mod = (file_info*)malloc(sizeof(file_info)*record[commit_num].num[3]);
			record[commit_num].copy = (file_info*)malloc(sizeof(file_info)*record[commit_num].num[4]);
			record[commit_num].becopy = (file_info*)malloc(sizeof(file_info)*record[commit_num].num[4]);
			record[commit_num].del = (file_info*)malloc(sizeof(file_info)*record[commit_num].num[5]);

			for(int i = 0; i < record[commit_num].num[2]; i++) {
				fread(&record[commit_num].add[i].namesize, sizeof(uint8_t), 1, fp);
				fread(record[commit_num].add[i].filename, sizeof(char), record[commit_num].add[i].namesize, fp);
				record[commit_num].add[i].filename[record[commit_num].add[i].namesize] = '\0';
			}

			for(int i = 0; i < record[commit_num].num[3]; i++) {
				fread(&record[commit_num].mod[i].namesize, sizeof(uint8_t), 1, fp);
				fread(record[commit_num].mod[i].filename, sizeof(char), record[commit_num].mod[i].namesize, fp);
				record[commit_num].mod[i].filename[record[commit_num].mod[i].namesize] = '\0';
			}
			for(int i = 0; i < record[commit_num].num[4]; i++) {
				fread(&record[commit_num].becopy[i].namesize, sizeof(uint8_t), 1, fp);
				fread(record[commit_num].becopy[i].filename, sizeof(char), record[commit_num].becopy[i].namesize, fp);
				record[commit_num].becopy[i].filename[record[commit_num].becopy[i].namesize] = '\0';
				fread(&record[commit_num].copy[i].namesize, sizeof(uint8_t), 1, fp);
				fread(record[commit_num].copy[i].filename, sizeof(char), record[commit_num].copy[i].namesize, fp);
				record[commit_num].copy[i].filename[record[commit_num].copy[i].namesize] = '\0';
			}
			for(int i = 0; i < record[commit_num].num[5]; i++) {
				fread(&record[commit_num].del[i].namesize, sizeof(uint8_t), 1, fp);
				fread(record[commit_num].del[i].filename, sizeof(char), record[commit_num].del[i].namesize, fp);
				record[commit_num].del[i].filename[record[commit_num].del[i].namesize] = '\0';
			}
	
			record[commit_num].file_md5 = (md5*)malloc(sizeof(md5)*record[commit_num].num[1]);
			for(int i = 0; i < record[commit_num].num[1]; i++) {
				fread(&record[commit_num].file_md5[i].namesize, sizeof(uint8_t), 1, fp);
				fread(record[commit_num].file_md5[i].filename, sizeof(char), record[commit_num].file_md5[i].namesize, fp);
				record[commit_num].file_md5[i].filename[record[commit_num].file_md5[i].namesize] = '\0';
				fread(record[commit_num].file_md5[i].hash, sizeof(unsigned char), 16, fp);
			}
			//for(int i = 0; i < 7; i++)
			//	printf("%d ", record[commit_num].num[i]);
			commit_num++;
			if(commit_num >= record_size) {
				realloc(record, sizeof(info)*(record_size+100));
				record_size += 100;
			}
		}
		fclose(fp);
	}
	if(argc == 4 && strcmp(argv[1], "log") == 0) {
		if(log_exist_id == -1)
			return 0;
		int lgnum = atoi(argv[2]);
		if(commit_num < lgnum)
			lgnum = commit_num;
		for(int i = 0; i < lgnum ; i++) {
			print_info(record[commit_num-i-1]);
			if(i != lgnum-1)
				printf("\n");
		}
	}
	if(argc == 3 && strcmp(argv[1], "status") == 0) {
		if(log_exist_id == -1) {
			printf("[new_file]\n");
			for(int i = 0; i < dir_num; i++) {
				if(strcmp(dname[i], ".loser_record") == 0)
					continue;
				else
					printf("%s\n", dname[i]);
			}
			printf("[modified]\n");
			printf("[copied]\n");
			printf("[deleted]\n");
			return 0;
		}
		md5 *n_files = (md5*)malloc(sizeof(md5)*1001);
		for(int i = 0; i < dir_num; i++) {
			n_files[i].namesize = (uint8_t)strlen(dname[i]);
			strcpy(n_files[i].filename, dname[i]);
			MD5_CTX sdf;
			MD5Init(&sdf);
			FILE *fp = fopen(dname2[i], "rb");
			if(fp == NULL)
				printf("error\n");
			fseek(fp, 0, SEEK_END);
			size_t f_len = ftell(fp);
			
			rewind(fp);
			
			unsigned char *encrypt = (unsigned char*)malloc(f_len*(sizeof(unsigned char))+1);
			encrypt[f_len] = '\0';
			fread(encrypt, sizeof(unsigned char), f_len, fp);
			unsigned char decrypt[16];
			MD5Update(&sdf,encrypt, f_len);
			MD5Final(&sdf, decrypt);
			for(int c = 0; c < 16; c++)
				n_files[i].hash[c] = decrypt[c];
			fclose(fp);
		}
		stat hoshizora = md5_cmp(record[commit_num-1].file_md5, n_files,
		record[commit_num-1].num[1], dir_num);
		
		printf("[new_file]\n");
		for(int i = 0; i < hoshizora.add; i++) {
			printf("%s\n", n_files[hoshizora.add_id[i]].filename);
		}
		printf("[modified]\n");
		for(int i = 0; i < hoshizora.mod; i++) {
			printf("%s\n", record[commit_num-1].file_md5[hoshizora.mod_id[i]].filename);
		}
		printf("[copied]\n");
		for(int i = 0; i < hoshizora.copy; i++) {
			printf("%s => %s\n", record[commit_num-1].file_md5[hoshizora.copy_id[i][0]].filename, n_files[hoshizora.copy_id[i][1]].filename);
		}
		printf("[deleted]\n");
		for(int i = 0; i < hoshizora.del; i++) {
			printf("%s\n", record[commit_num-1].file_md5[hoshizora.del_id[i]].filename);
		}
	}
	if(argc == 3 && strcmp(argv[1], "commit") == 0) {
		md5 *n_files = (md5*)malloc(sizeof(md5)*1001);
		for(int i = 0; i < dir_num; i++) {
			n_files[i].namesize = (uint8_t)strlen(dname[i]);
			strcpy(n_files[i].filename, dname[i]);
			MD5_CTX sdf;
			MD5Init(&sdf);
			FILE *fp = fopen(dname2[i], "rb");
			if(fp == NULL)
				printf("error\n");
			fseek(fp, 0, SEEK_END);
			size_t f_len = ftell(fp);
			
			rewind(fp);
			
			unsigned char *encrypt = (unsigned char*)malloc(f_len*(sizeof(unsigned char))+1);
			encrypt[f_len] = '\0';
			fread(encrypt, sizeof(unsigned char), f_len, fp);
			unsigned char decrypt[16];
			MD5Update(&sdf,encrypt, f_len);
			MD5Final(&sdf, decrypt);
			for(int c = 0; c < 16; c++)
				n_files[i].hash[c] = decrypt[c];
			fclose(fp);
		}
		stat hoshizora;
		FILE *fp ;
		uint32_t dr = dir_num - 1;
		if(log_exist_id != -1) {
			hoshizora = md5_cmp(record[commit_num-1].file_md5, n_files, record[commit_num-1].num[1], dir_num);
			fp = fopen(dname2[log_exist_id], "ab");
		}
		else {
			dr = dir_num;
			md5* empty;
			hoshizora = md5_cmp(empty, n_files, 0, dir_num);
			char re[20] = "\.loser_record", path[512];
			strcpy(path, argv[2]);
			strcat(path, re);
			fp = fopen(path, "ab");
		}
		uint32_t wr = (uint32_t)commit_num + 1;
		fwrite( &wr, sizeof(uint32_t), 1, fp);

		fwrite( &dr , sizeof(uint32_t), 1, fp);
		fwrite( &(hoshizora.add) , sizeof(uint32_t), 1, fp);
		fwrite( &(hoshizora.mod) , sizeof(uint32_t), 1, fp);
		fwrite( &(hoshizora.copy) , sizeof(uint32_t), 1, fp);
		fwrite( &(hoshizora.del) , sizeof(uint32_t), 1, fp);
		uint32_t size = 28;
		for(int i = 0; i < hoshizora.add; i++)
			size += (n_files[hoshizora.add_id[i]].namesize + 1);
		for(int i = 0; i < hoshizora.mod; i++)
			size += 1 + record[commit_num-1].file_md5[hoshizora.mod_id[i]].namesize;
		for(int i = 0; i < hoshizora.copy; i++)
			size += 2 + record[commit_num-1].file_md5[hoshizora.copy_id[i][0]].namesize + n_files[hoshizora.copy_id[i][1]].namesize;
		for(int i = 0; i < hoshizora.del; i++)
			size += 1 + record[commit_num-1].file_md5[hoshizora.del_id[i]].namesize;
		for(int i = 0; i < dir_num; i++)
			if(strcmp(n_files[i].filename, ".loser_record") == 0)
				continue;
			else
				size += 17 + n_files[i].namesize;
		fwrite(&size, sizeof(uint32_t), 1, fp);
		
		for(int i = 0; i < hoshizora.add; i++) {
			uint8_t cat = n_files[hoshizora.add_id[i]].namesize;
			fwrite(&cat, sizeof(uint8_t), 1, fp);
			fwrite(n_files[hoshizora.add_id[i]].filename, sizeof(char), cat, fp);
		}
		for(int i = 0; i < hoshizora.mod; i++) {
			uint8_t cat = record[commit_num-1].file_md5[hoshizora.mod_id[i]].namesize;
			fwrite(&cat, sizeof(uint8_t), 1, fp);
			fwrite(record[commit_num-1].file_md5[hoshizora.mod_id[i]].filename, sizeof(char), cat, fp);
		}
		for(int i = 0; i < hoshizora.copy; i++) {
			uint8_t cat = record[commit_num-1].file_md5[hoshizora.copy_id[i][0]].namesize;
			fwrite(&cat, sizeof(uint8_t), 1, fp);
			fwrite(record[commit_num-1].file_md5[hoshizora.copy_id[i][0]].filename, sizeof(char), cat, fp);
			
			cat = n_files[hoshizora.copy_id[i][1]].namesize;
			fwrite(&cat, sizeof(uint8_t), 1, fp);
			fwrite(n_files[hoshizora.copy_id[i][1]].filename, sizeof(char), cat, fp);
		}
		for(int i = 0; i < hoshizora.del; i++) {
			uint8_t cat = record[commit_num-1].file_md5[hoshizora.del_id[i]].namesize;
			fwrite(&cat, sizeof(uint8_t), 1, fp);
			fwrite(record[commit_num-1].file_md5[hoshizora.del_id[i]].filename, sizeof(char), cat, fp);
		}
		for(int i = 0; i < dir_num; i++)
			if(strcmp(n_files[i].filename, ".loser_record") == 0)
				continue;
			else {
				fwrite(&n_files[i].namesize, sizeof(uint8_t), 1, fp);
				fwrite(n_files[i].filename, sizeof(char), n_files[i].namesize, fp);
				fwrite(n_files[i].hash, sizeof(unsigned char), 16, fp);
			}
	}
}