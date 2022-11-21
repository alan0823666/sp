#include <stdio.h>
#include <string.h>
char buffer[15000001];
int main(int argc, char *argv[]){
	if(argc != 3 && argc != 2) {
		fprintf(stderr, "%s", "error\n");
		return 0;
	}
	int table[256] = {0};
	for(int i = 0; i < strlen(argv[1]); i++)
		table[argv[1][i]] = 1;
	if(argc == 3) {
		FILE *fp;
		fp = fopen(argv[2],"r");
		if(fp == NULL) {
			fprintf(stderr, "%s", "error\n");
			return 0;
		}
		long long int num[256] = {0};
		while(fgets(buffer, 15000000, fp) != NULL) {
			int rin = strlen(buffer);
			for(int i = 0; i < rin; i++)
				num[buffer[i]]++;
			if(buffer[rin-1] == '\n')  {
				long long int sum = 0;
				for(int i = 0; i < 256; i++)
					if(table[i])
						sum += num[i];
				printf("%lld\n", sum);
				for(int i = 0; i < 256; i++)
					num[i] = 0;
			}
		}
		fclose(fp);
		return 0;
	}
	else if(argc == 2) {
		long long int num[256] = {0};
		while(fgets(buffer, 15000000, stdin) != NULL) {
			int rin = strlen(buffer);
			for(int i = 0; i < rin; i++)
				num[buffer[i]]++;
			if(buffer[rin-1] == '\n')  {
				long long int sum = 0;
				for(int i = 0; i < 256; i++)
					if(table[i])
						sum += num[i];
				printf("%lld\n", sum);
				for(int i = 0; i < 256; i++)
					num[i] = 0;
			}
		}
	}
}