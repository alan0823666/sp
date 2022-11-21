#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main() {
	FILE *fp = fopen("12.txt", "wb");
	uint32_t s[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	fwrite(s, sizeof(uint32_t), 16, fp);
	fclose(fp);
}