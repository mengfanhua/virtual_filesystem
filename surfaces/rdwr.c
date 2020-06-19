#include "virtual_system.h"
#include<malloc.h>
#include<string.h>



char* read(int dnode_index) {
	//读出一块数据区
	char *p;
	p = (char *)malloc(sizeof(char)*BLOCK_SIZE);
	fseek(fp, DATA_START + dnode_index*BLOCK_SIZE, SEEK_SET);
	fread(p, 1, BLOCK_SIZE, fp);
	return p;
}

void write(int dnode_index, char* content) {
	//写入一块数据区
	fseek(fp, DATA_START + dnode_index*BLOCK_SIZE, SEEK_SET);
	if (strlen(content) >= BLOCK_SIZE) {
		fwrite(content, 1, BLOCK_SIZE, fp);
	}
	else {
        fwrite(content, 1, strlen(content), fp);
        char a = '\0';
        fwrite(&a, 1, 1, fp);
	}
}
