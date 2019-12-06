#include "main.h"

DataCluster crtdir(DataCluster cluster, DirEntry folder) {
	int i = 0;
	while (cluster.dir[i].filename[0] != 0) i++;
	cluster.dir[i] = folder;
	return cluster;
}

DirEntry newdir(char *filename) {
	DirEntry folder;
	strncpy((char*)folder.filename,filename,17*sizeof(char));
	folder.attributes = 1;
	folder.firstblock = findSpace();
	gFat[folder.firstblock] = 0xFFFF;
	folder.size = 0x0400;
	return folder;
}

int dirSET(DataCluster cluster, char *path) {
	int block = -1;
	for (size_t i=0; i < ENTRYBYCLUSTER; i++) {
		if (strcmp(path,(char*)cluster.dir[i].filename) == 0)
			block = cluster.dir[i].firstblock;
	}
	return block;
}

void prtls(DataCluster cluster, int block, char *path) {
	int space = 0;
	printf("\u250C 0x%04X "BOLD"%s\n"NORM, block, path);
	for (size_t i=0; i < ENTRYBYCLUSTER; i++) {
		if (cluster.dir[i].filename[0] != 0x0000) {
			space++;
			printf("\u251C\u2574 0x%04X ", cluster.dir[i].firstblock);
			printf("%dB ", cluster.dir[i].size);
			if (cluster.dir[i].attributes == 1)
				printf(BOLD"%s"NORM, cluster.dir[i].filename);
			else 
				printf("%s", cluster.dir[i].filename);
			putchar(0x0A);
		}
	}
	printf("\u2514 cap: %d/32\n", space);
}

int fatexist(void) {
	FILE *fp = fopen(FATNAME,"r");
	if (fp != NULL) {
		char *app = program_invocation_short_name;
		char *err = strerror(EEXIST);
		fprintf(stderr,"%s: %s: %s\n",app,FATNAME,err);
		fclose(fp);
		return 1;
	}
	return 0;
}

int format(void) {
	int setfmt = -1;
	printf("Deseja Formatar? (s/n) ");
	char c = getchar();
	clrBf();
	switch (c) {
		case 'S': case 's': setfmt = 1; break;
		case 'N': case 'n': setfmt = 0; break;
		default: format();
	}
	return setfmt;
}

// stackoverflow.com/questions/3969871
void clrBf(void) {
	char c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
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