#include "main.h"

DataCluster readCL(int index) {
	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	DataCluster cluster;
	fseek(fp,(CLUSTERSIZE*index),SEEK_SET);
	fread(&cluster,sizeof(DataCluster),1,fp);

	fclose(fp);
	return cluster;
}

void writeCL(int index, DataCluster cluster) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	fseek(fp,(CLUSTERSIZE*index), SEEK_SET);
	fwrite(&cluster,CLUSTERSIZE,1,fp);

	fclose(fp);
}

void writeFAT(void) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	fseek(fp,CLUSTERSIZE,SEEK_SET);
	fwrite(&g_fat,NUMCLUSTER,1,fp);

	fclose(fp);
}

int findSpace(void) {
	int i = 0;
	while (g_fat[i] != 0 && i < NUMCLUSTER) i++;
	if (i == NUMCLUSTER)
		return -1;
	return i;
}