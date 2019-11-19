#include "main.h"

// djb2 http://www.cse.yorku.ca/~oz/hash.html
uint64_t hashcmd(char *str) {
	uint64_t hash = 0x1505; // 5381
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
		// hash * 33 + c (Magic number)
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
	dbgtokn(tokens);
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

	uint16_t argc = argcount(argv);
	uint64_t hash = hashcmd(argv[0]);
	int status = 0;

	#ifdef DEBUG
	dbgargv(hash,argc,argv);
	#endif

	switch (hash) {
	case INIT:   status =   init(argc);      break;
	case LOAD:   status =   load(argc);      break;
	case LS:     status =     ls(argc,argv); break;
	case MKDIR:  status =  mkdir(argc,argv); break;
	case CREATE: status = create(argc,argv); break;
	case UNLINK: status = unlink(argc,argv); break;
	case WRITE:  status =  write(argc,argv); break;
	case APPEND: status = append(argc,argv); break;
	case READ:   status =   read(argc,argv); break;
	case HELP:   help();                     break;
	case EXIT:   shsair();                   exit(EXIT_SUCCESS);
	default:     status = cmderr(cmd);       break;
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

int argerr(int argc, int args) {
	char *app = program_invocation_short_name;
	char *err = strerror(EINVAL);
	if (argc != args) {
		fprintf(stderr,"%s: %s\n",app,err);
		return 1;
	}
	return 0;
}

int cmderr(char *cmd) {
	char *app = program_invocation_short_name;
	char *err = "comando nao encontrado";
	fprintf(stderr,"%s: %s: %s\n",app,err,cmd);
	puts("digite 'help' para ajuda");
	return 1;
}

void shsair(void) {
	puts("Saindo...");
}

void dbgtokn(char **tokens) {
	int i = 0;
	printf(CYEL);
	while (tokens[i] != NULL) {
		printf("tokens[%d] = %s\n",i,tokens[i]);
		i++;
	}
	printf(CRST);
}

void dbgargv(uint64_t hash, uint16_t argc, char **argv) {
	printf(CYEL);
	printf("  argv[0]-> %s\n", argv[0]);
	printf("  cmdhash-> %ld\n",hash);
	printf("     argc-> %d\n", argc);
	printf(CRST);
}