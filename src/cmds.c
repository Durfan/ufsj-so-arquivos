#include "main.h"

int init(uint16_t argc) {
	if (argerr(argc,1))
		return 1;

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

	memset(g_rootdir,0x00,sizeof(g_rootdir));
	fwrite(&g_rootdir,sizeof(g_rootdir),1,fp);

	for (int i=0; i < 4086; ++i)
		fwrite(&g_clusters,sizeof(DataCluster),1,fp);

	fclose(fp);
	return 0;
}

int load(uint16_t argc) {
	if (argerr(argc,1))
		return 1;

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

/*
int ls(uint16_t argc, char **argv) {
	if (argerr(argc,2))
		return 1;

	return 0;
}
*/

int mkdir(uint16_t argc, char **argv) {
	if (argerr(argc,2))
		return 1;

	char *delim = "/";
	char **path = tkenizer(argv[1],delim);

	DataCluster root = readCL(9);
	DirEntry folder;
	memset(folder.filename,'\0',18*sizeof(char));
	memset(folder.reserved,0,7*sizeof(char));
	strncpy((char*)folder.filename,path[0],17*sizeof(char));
	folder.attributes = 1;
	folder.firstblock = 0;
	folder.size = 0;

	for (long unsigned i=0; i < ENTRYBYCLUSTER; i++) {
		if (root.dir[i].firstblock == 0) {
			root.dir[i] = folder;
			break;
		}
	}

	writeCL(9,root);

	free(path);

	return 0;
}

/*
int create(uint16_t argc, char **argv) {

	return 0;
}

int unlink(uint16_t argc, char **argv) {

	return 0;
}

int write(uint16_t argc, char **argv) {

	return 0;
}

int append(uint16_t argc, char **argv) {

	return 0;
}

int read(uint16_t argc, char **argv) {

	return 0;
}
*/

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