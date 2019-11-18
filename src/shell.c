#include "main.h"

uint64_t hashcmd(char *cmd) { // http://www.cse.yorku.ca/~oz/hash.html
	uint64_t hash = 0x1505; // 5381
	int c;
	while ((c = *cmd++))
		hash = ((hash << 5) + hash) + c; // hash * 33 + c
	return hash;
}

void shell(void) {
	char cmd[MAXCMD];
	int status = 0;
	do {

		prompt(status);
		if (fgets(cmd,MAXCMD,stdin) && strcmp(cmd,"\n")) {
			status = commands(cmd);
		}
		else
			status = 0;
		freebuf(cmd);

	} while (!feof(stdin));
	shsair();
}

char **tkenizer(char *input, char *delim) {
	char **tokens = malloc(32 * sizeof(char*));
	if (tokens == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	char *parsed;
	unsigned index = 0;
	parsed = strtok(input,delim);
	while (parsed != NULL) {
		tokens[index] = parsed;
		index++;
		parsed = strtok(NULL,delim);
	}

	tokens[index] = NULL;

	#ifdef DEBUG
	dbugtkn(tokens);
	#endif

	return tokens;
}

unsigned argcount(char **argv) {
	unsigned index = 0;
	while (argv[index] != NULL)
		index++;
	return index;
}

int commands(char *cmd) {
	char *delim = " \n";
	char **argv = tkenizer(cmd,delim);
	if (argv == NULL)
		return argerr();

	uint16_t argc = argcount(argv);
	uint64_t hash = hashcmd(argv[0]);
	int status = 0;

	#ifdef DEBUG
	printf("argv[0]: %s\n", argv[0]);
	printf("cmdhash: %ld\n", hash);
	printf("   argc: %d\n", argc);
	#endif

	switch (hash) {
	case INIT:
		status = init(argc);
		break;

	case LOAD:
		status = load(argc);
		break;

	case LS:
		status = ls(argc,argv);
		break;

	case MKDIR:
		status = mkdir(argc,argv);
		break;

	case HELP:
		help();
		break;

	case EXIT:
		shsair();
		exit(EXIT_SUCCESS);
	
	default:
		status = cmderr(cmd);
		break;
	}

	free(argv);
	return status;
}

void prompt(int status) {
	printf("%s:"BOLD,program_invocation_short_name);
	if (status == 1)
		printf(CRED"$ "CRST);
	else	
		printf("$ ");
	printf(NORM);
}

int argerr(void) {
	printf("%s: ", program_invocation_short_name);
	puts(strerror(EINVAL));
	return 1;
}

int cmderr(char *cmd) {
	printf("%s: ",program_invocation_short_name);
	printf("comando nao encontrado: %s\n",cmd);
	puts("digite 'help' para ajuda");
	return 1;
}

void shsair(void) {
	puts("Saindo...");
}

void dbugtkn(char **tokens) {
	int i = 0;
	while (tokens[i] != NULL) {
		printf("tokens[%d] = %s\n",i,tokens[i]);
		i++;
	}
}