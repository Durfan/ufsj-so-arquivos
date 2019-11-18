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

	DataCluster *block = readCL(9);
	DirEntry new;

	// tendo o diretorio no qual queremos criar o novo (token)
	// basta verificar se nao existe um arquivo com este mesmo nome
	// verificar se possui um bloco livre no diretório e na fat
	for (long unsigned i=0; i < ENTRYBYCLUSTER; i++) {

		if (block->dir[i].firstblock == 0) {
			
			int index_fat = findSpace();
			g_fat[index_fat] = 0xffff;
			memcpy(new.filename,argv[1],sizeof(char)*strlen(argv[1]));
			new.attributes = 1;
			new.firstblock = index_fat;
			new.size = 0;

			block->dir[i] = new;
			writeCL(9,block);
			writeFAT();
			break;
		}
	}

	return 0;
}

int create(void) {

	return 0;
}

int unlink(void) {

	return 0;
}

int write(void) {

	return 0;
}

int append(void) {

	return 0;
}

int read(void) {

	return 0;
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