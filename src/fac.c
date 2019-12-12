#include "main.h"

DataCluster rdClster(int index) {
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

void wrClster(int index, DataCluster cluster) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	fseek(fp,(CLUSTER*index),SEEK_SET);
	fwrite(&cluster,CLUSTER,1,fp);

	fclose(fp);
}

void rmClster(int index) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	DataCluster cluster = { 0x00 };
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
	fwrite(&gFat,NUMCLUSTERS,1,fp);

	fclose(fp);
}

int freeAddr(void) {
	int i = 10;
	while (gFat[i] != 0x0000 && i < NUMCLUSTERS) i++;
	if (i == NUMCLUSTERS)
		return -1;
	return i;
}

void fatimg(void) {
	char output[0x100];
	strcpy(output,FATNAME);
	strcat(output,".PPM");

	FILE *fp = fopen(output,"w+");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		return;
	}

	fprintf(fp, "P3\n");
	fprintf(fp, "128 32\n");
	fprintf(fp, "255\n");

	for (int i=0; i < NUMCLUSTERS; i++) {
		if (gFat[i] == 0xfffd)
			fprintf(fp, "0 0 0");
		else if (gFat[i] == 0xfffe)
			fprintf(fp, "255 255 0");
		else if (gFat[i] == 0xffff)
			fprintf(fp, "0 0 255");
		else if (gFat[i] >= 0x0001 && gFat[i] <= 0xfffc)
			fprintf(fp, "0 255 0");
		else if (gFat[i] == 0x0000)
			fprintf(fp, "255 255 255");
		fprintf(fp, " ");
	}

	fclose(fp);
}