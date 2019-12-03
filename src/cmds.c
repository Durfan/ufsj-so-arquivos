#include "main.h"

int init(uint16_t argc) {
	if (argerr(argc,1))
		return 1;

	if (fatexist() && format() == 0)
		return 0;

	FILE *fp = fopen(FATNAME,"wb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	memset(gBootblock,0xBB,sizeof(gBootblock));
	fwrite(&gBootblock,sizeof(gBootblock),1,fp);

	gFat[0] = 0xFFFD;
	for (int i=1; i < 9; ++i)
		gFat[i] = 0xFFFE;
	gFat[9] = 0xFFFF;
	for (int i=10; i < NUMCLUSTERS; ++i)
		gFat[i] = 0x0000;
	fwrite(&gFat,sizeof(gFat),1,fp);

	memset(gRootdir,0x00,sizeof(gRootdir));
	fwrite(&gRootdir,sizeof(gRootdir),1,fp);

	for (int i=0; i < 4086; ++i)
		fwrite(&gClusters,sizeof(DataCluster),1,fp);

	fclose(fp);
	gFatplug = true;
	return 0;
}

int load(uint16_t argc) {
	if (argerr(argc,1))
		return 1;

	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		char *app = program_invocation_short_name;
		char *err = strerror(ENODEV);
		fprintf(stderr,"%s: %s\n",app,err);
		puts("digite 'help' para ajuda");
		return 1;
	}

	fseek(fp,sizeof(gBootblock),SEEK_SET);
	fread(gFat,sizeof(gFat),1,fp);
	fread(gRootdir,sizeof(gRootdir),1,fp);
	fclose(fp);
	gFatplug = true;
	return 0;
}


int ls(uint16_t argc, char **argv) {
	if (argerr(argc,2))
		return 1;
	
	if (gFatplug == false) {
		plugerr();
		return 1;
	}

	DataCluster cluster = readCL(9);
	int i=0, exists, block = 9;
	char *argv1 = NULL;
	char **path = NULL;
	char *delim = "/";

	if (argc > 1) {
		argv1 = strdup(argv[1]);
		path = tkenizer(argv[1],delim);

		while (path[i] != NULL) {
			cluster = readCL(block);
			exists  = dirSET(cluster,path[i]);
			if (exists < 0) {
				fprintf(stderr,"%s\n",strerror(ENOENT));
				return 1;
			}
			else
				block = exists;
			i++;
		}
	}
	else
		argv1 = strdup("root");

	int space = 0;
	cluster = readCL(block);
	printf("\u250C 0x%04X "BOLD"%s\n"NORM, block, argv1);
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

	free(argv1);
	free(path);
	return 0;
}


int mkdir(uint16_t argc, char **argv) {
	if (argerr(argc,2))
		return 1;

	if (gFatplug == false) {
		plugerr();
		return 1;
	}

	DataCluster cluster;
	DirEntry folder;
	int i=0, exists, block = 9;

	char *delim = "/";
	char **path = tkenizer(argv[1],delim);

	while (path[i] != NULL) {
		cluster = readCL(block);
		exists  = dirSET(cluster,path[i]);
		if (exists < 0) {
			folder  = newdir(path[i]);
			cluster = crtdir(cluster,folder);
			writeCL(block,cluster);
			writeFAT();
			block = folder.firstblock;
		}
		else
			block = exists;
		i++;
	}

	free(path);
	return 0;
}

DataCluster crtdir(DataCluster cluster, DirEntry folder) {
	int i = 0;
	while (cluster.dir[i].filename[0] != 0) i++;
	cluster.dir[i] = folder;
	return cluster;
}

int dirSET(DataCluster cluster, char *path) {
	int block = -1;
	for (size_t i=0; i < ENTRYBYCLUSTER; i++) {
		if (strcmp(path,(char*)cluster.dir[i].filename) == 0)
			block = cluster.dir[i].firstblock;
	}
	return block;
}

DirEntry newdir(char *filename) {
	DirEntry folder;
	memset(folder.filename,'\0',18*sizeof(char));
	memset(folder.reserved,0,7*sizeof(char));
	strncpy((char*)folder.filename,filename,17*sizeof(char));
	folder.attributes = 1;
	folder.firstblock = findSpace();
	gFat[folder.firstblock] = 0xFFFF;
	folder.size = 0x0400;
	return folder;
}


int create(uint16_t argc, char **argv) {
	if (argerr(argc,3))
		return 1;

	if (gFatplug == false) {
		plugerr();
		return 1;
	}

	size_t len = strlen(argv[1]);
	printf("Tamanho: %ld\n", len);

	char str1[CLUSTER];
	char str2[CLUSTER];
	memcpy(str1,argv[1], CLUSTER);
	memcpy(str2,argv[1] + CLUSTER, CLUSTER);

	printf("%s\n", str1);

	return 0;
}

/*

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

int format(void) {
	int setfmt = -1;
	printf("Deseja Formatar? (s/n) ");
	char c = getchar();
	clrBuff();
	switch (c) {
		case 'S': case 's': setfmt = 1; break;
		case 'N': case 'n': setfmt = 0; break;
		default: format();
	}
	return setfmt;
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

// stackoverflow.com/questions/3969871
void clrBuff(void) {
	char c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

void plugerr(void) {
	char *app = program_invocation_short_name;
	char *err = strerror(ENXIO);
	fprintf(stderr,"%s: %s\n",app,err);
}