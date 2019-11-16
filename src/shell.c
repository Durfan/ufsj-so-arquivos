#include "main.h"

unsigned long hashcmd(char *cmd) {
	unsigned long hash = 5381;
	int c;
	while ((c = *cmd++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

int init(void) {
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

int load(void) {
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

void shell(void) {
	char cmd[MAXCMD];
	int status = 0;

	do {
		prompt(status);

		if (fgets(cmd,MAXCMD,stdin) && strcmp(cmd,"\n")) {
			status = commands(cmd);
		}

		freebuf(cmd);

	} while (!feof(stdin));

	shsair();
}

char **tkenizer(char *input) {
	char **command = malloc(4 * sizeof(char*));
	if (command == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	char *parsed;
	unsigned index = 0;

	parsed = strtok(input," \n");
	while (parsed != NULL) {
		command[index] = parsed;
		index++;
		if (index > 3) {
			free(command);
			return NULL;
		}
		parsed = strtok(NULL," \n");
	}

	command[index] = NULL;
	return command;
}

unsigned argcount(char **argv) {
	unsigned index = 0;
	while (argv[index] != NULL) {
		index++;
	}
	return index;
}

int commands(char *cmd) {
	char **argv = tkenizer(cmd);
	if (argv == NULL) {
		printf("%s: ", program_invocation_short_name);
		puts(strerror(E2BIG));
		return 1;
	}
	unsigned argc = argcount(argv);
	unsigned long hash = hashcmd(argv[0]);
	int status = 0;

	#ifdef DEBUG
	printf("argv[0]: %s\n", argv[0]);
	printf("cmdhash: %ld\n", hash);
	printf("   argc: %d\n", argc);
	#endif

	switch (hash) {
	case INIT:
		status = init();
		break;

	case LOAD:
		status = load();
		break;

	case HELP:
		help();
		break;

	case EXIT:
		shsair();
		exit(EXIT_SUCCESS);
	
	default:
		cmderr(cmd);
		status = 1;
		break;
	}

	free(argv);
	return status;
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

void prompt(int status) {
	printf("%s:"BOLD,program_invocation_short_name);
	if (status == 1)
		printf(CRED"$ "CRST);
	else	
		printf("$ ");
	printf(NORM);
}

void cmderr(char *cmd) {
	printf("%s: ",program_invocation_short_name);
	printf("comando nao encontrado: ");
	printf("%s\n",cmd);
}

void shsair(void) {
	puts("Saindo...");
}