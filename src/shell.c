#include "main.h"

unsigned long hashcmd(char *cmd) {
	unsigned long hash = 5381;
	int c;
	while ((c = *cmd++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

void init(void) {
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
}

void load(void) {
	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}
	fseek(fp,sizeof(g_bootblock),SEEK_SET);
	fread(g_fat,sizeof(g_fat),1,fp);
	fread(g_rootdir,sizeof(g_rootdir),1,fp);
	fclose(fp);
}

void shell(void) {
	char cmd[MAXCMD];
	int status = 0;

	do {
		prompt(status);

		if (fgets(cmd,MAXCMD,stdin) && strcmp(cmd,"\n")) {
			tkenizer(cmd);
		}

		freebuf(cmd);

	} while (!feof(stdin));

	putchar(0x0A);
}

void tkenizer(char *cmd) {
	char *token = strtok(cmd," \n");
	unsigned long command = hashcmd(token);

	#ifdef DEBUG
	printf("cmdhash: %ld\n", command);
	#endif

	switch (command) {
	case INIT:
		init();
		break;

	case LOAD:
		load();
		break;

	case HELP:
		help();
		break;

	case EXIT:
		shsair();
		exit(EXIT_SUCCESS);
	
	default:
		cmderr(cmd);
		break;
	}
}

void help(void) {
	puts("      inicializar o drive:   init");
	puts("         carregar o drive:   load");
	puts("         listar diretorio:     ls [/caminho/diretorio]");
	puts("          criar diretorio:  mkdir [/caminho/diretorio]");
	puts("            criar arquivo: create [/caminho/arquivo]");
	puts("excluir arquivo/diretorio: unlink [/caminho/arquivo]");
	puts("      escrever no arquivo:  write [/caminho/arquivo]");
	puts("        anexar em arquivo: append [/caminho/arquivo]");
	puts("           ler um arquivo:   read [/caminho/arquivo]");
	puts("            exibe a ajuda:   help");
	puts("                     sair:   exit");
}

void prompt(int status) {
	printf("> ");
}

void cmderr(char *cmd) {
	printf("%s: ",program_invocation_short_name);
	printf("comando nao encontrado: ");
	printf("%s\n",cmd);
}

void shsair(void) {
	puts("Saindo...");
}