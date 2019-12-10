#include "main.h"

DataCluster crtdir(DataCluster cluster, DirEntry entry) {
	int i = 0;
	while (cluster.dir[i].filename[0] != 0) i++;
	cluster.dir[i] = entry;
	return cluster;
}

DirEntry newentry(char *filename, uint8_t attr) {
	DirEntry entry;
	memset(entry.filename,0x00,sizeof(entry.filename));
	memset(entry.reserved,0x00,sizeof(entry.reserved));
	strncpy((char*)entry.filename,filename,17*sizeof(char));
	entry.attributes = attr;
	entry.firstblock = freeAddr();
	gFat[entry.firstblock] = 0xFFFF;
	entry.size = 0x00000400;
	return entry;
}

DirEntry getentry(DataCluster cluster, char *path, int *idx) {
	DirEntry entry = { 0 };
	char *filename = NULL;
	for (size_t i=0; i < ENTRYBYCLUSTER; i++) {
		filename = (char*)cluster.dir[i].filename;
		if (strcmp(path,filename) == 0) {
			(*idx) = i;
			entry  = cluster.dir[i];
		}
	}
	return entry;
}

void prtls(DataCluster cluster, char *path, int block) {
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

int maxdname(char **path) {
	int i = 0;
	bool max = false;
	while (path[i] != NULL) {
		if (strlen(path[i]) > 17)
			max = true;
		i++;
	}
	if (max) {
		free(path);
		erro(ENAMETOOLONG);
		return 1;
	}
	return 0;
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
	int answer = -1;
	printf("Deseja Formatar? (s/n) ");
	char c = getchar();
	clrBuff();
	switch (c) {
		case 'S': case 's': answer = 1; break;
		case 'N': case 'n': answer = 0; break;
		default: format();
	}
	return answer;
}

// stackoverflow.com/questions/3969871
// porque? setbuf,fflush nao FUNCIONA
// mas essa tosqueira, funciona
void clrBuff(void) {
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

bool isLoaded(void) {
	return gFatplug;
}