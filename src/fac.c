#include "main.h"

DataCluster readCL(int index) {
	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	DataCluster cluster;
	fseek(fp,(CLUSTER*index),SEEK_SET);
	fread(&cluster,sizeof(DataCluster),1,fp);

	fclose(fp);
	return cluster;
}

void writeCL(int index, DataCluster cluster) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	fseek(fp,(CLUSTER*index),SEEK_SET);
	fwrite(&cluster,CLUSTER,1,fp);

	fclose(fp);
}

void writeFAT(void) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	fseek(fp,CLUSTER,SEEK_SET);
	fwrite(&g_fat,NUMCLUSTERS,1,fp);

	fclose(fp);
}

int findSpace(void) {
	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}
	fseek(fp,CLUSTER,SEEK_SET);
	uint16_t ptr;
	int index = -1;
	for (int i=0; i < NUMCLUSTERS; ++i) {
		fread(&ptr,sizeof(ptr),1,fp);
		if (ptr == 0x0000 && index == -1)
			index = i;
	}
	fclose(fp);
	return index;
}