#include "main.h"

int init(uint16_t argc) {
	if (argc != 1) {
		argerr();
		return 1;
	}

	FILE *fp = fopen(FATNAME,"wb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	for (int i=0; i < 2; ++i)
		g_bootblock[i] = 0x00BB;

	fwrite(&g_bootblock,sizeof(g_bootblock),1,fp);

	g_fat[0] = 0xFFFD;
	for (int i=1; i < 9; ++i)
		g_fat[i] = 0xFFFE;

	g_fat[9] = 0xFFFF;
	for (int i=10; i < NUMCLUSTER; ++i)
		g_fat[i] = 0x0000;

	fwrite(&g_fat,sizeof(g_fat),1,fp);
	fwrite(&g_rootdir,sizeof(g_rootdir),1,fp);

	for (int i=0; i < 4086; ++i)
		fwrite(&g_clusters,sizeof(DataCluster),1,fp);

	fclose(fp);
	return 0;
}

int load(uint16_t argc) {
	if (argc != 1) {
		argerr();
		return 1;
	}

	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	fseek(fp,sizeof(g_bootblock),SEEK_SET);
	fread(g_fat,sizeof(g_fat),1,fp);
	fread(g_rootdir,sizeof(g_rootdir),1,fp);
	fclose(fp);

	return 0;
}

int ls(uint16_t argc, char **argv) {
	if (argc != 2) {
		argerr();
		return 1;
	}

	return 0;
}

int mkdir(uint16_t argc, char **argv) {
	if (argc != 2) {
		argerr();
		return 1;
	}

	return 0;
}

int create(void) {

}

int unlink(void) {
	
}

int write(void) {
	
}

int append(void) {
	
}

int read(void) {
	
}

void help(void) {
	puts("      inicializar o drive:   init");
	puts("         carregar o drive:   load");
	puts("         listar diretorio:     ls [/caminho/diretorio]");
	puts("          criar diretorio:  mkdir [/caminho/diretorio]");
	puts("            criar arquivo: create [/caminho/arquivo]");
	puts("excluir arquivo/diretorio: unlink [/caminho/arquivo]");
	puts("      escrever no arquivo:  write \"string\" [/caminho/arquivo]");
	puts("        anexar em arquivo: append \"string\" [/caminho/arquivo]");
	puts("           ler um arquivo:   read [/caminho/arquivo]");
	puts("            exibe a ajuda:   help");
	puts("                     sair:   exit");
}

DataCluster *readCL(int index) {
	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		return NULL;
	}

	DataCluster *cluster;
	fseek(fp,(CLUSTERSIZE*index),SEEK_SET);
	fread(cluster,sizeof(DataCluster),1,fp);

	fclose(fp);
	return cluster;
}

void writeCL(int index, DataCluster *cluster) {
	FILE *fp = fopen(FATNAME,"rb+");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	fseek(fp,(CLUSTERSIZE*index), SEEK_SET);
	fwrite(cluster,CLUSTERSIZE,1,fp);

	fclose(fp);
}

int findSpace(void) {
	int i = 0;
	while (g_fat[i] != 0 && i < NUMCLUSTER) i++;
	if (i == NUMCLUSTER)
		return -1;
	return i;
}