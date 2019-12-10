#include "main.h"

// djb2 http://www.cse.yorku.ca/~oz/hash.html
// pra que? porque strings.h e um saco, prefiro
// um inteiro de 64bits do que um strcomp
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
		if (fgets(cmd,MAXCMD,stdin) && strcmp(cmd,"\n"))
			status = commands(cmd);
		else
			status = 0;
		freebuf(cmd);

	} while (!feof(stdin));
	shsair();
}

// Automato simples para um parser da cmdline
char **cmdparse(uint16_t *argc, char *cmd) {
	size_t tks = 0;
	char *ptr = cmd;
	while ((ptr = strchr(ptr,'"')) != NULL)
		tks++, ptr++;
	if (tks % 2 != 0) {
		eastere();
		return NULL;
	}

	char **arg = calloc(32,sizeof(char*));
	if (arg == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	int i = 0, index = 0;
	bool quote = false;
	arg[index] = &cmd[0];
	tks = 1;

	while (cmd[i] != '\n' && tks < 32) {
		if (cmd[i] == ' ' && !quote) {
			while (cmd[i] == ' ')
				cmd[i++] = '\0';
			if (cmd[i] == '"') {
				cmd[i++] = '\0';
				quote = !quote;
			} else if (cmd[i] == '\n')
				continue;
			arg[++index] = &cmd[i]; tks++;
		}
		if (cmd[i] == '"') {
			cmd[i] = '\0';
			quote = !quote;
		}
		i++;
	}

	cmd[i] = '\0';
	arg[++index] = NULL;
	(*argc) = tks;

	if (tks == 32) {
		free(arg);
		erro(E2BIG);
		return NULL;
	}
	return arg;
}

// tokenizador generico
char **tkenizer(char *input, char *delim) {
	char **tokens = calloc(32,sizeof(char*));
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

int commands(char *cmd) {
	int status = 0;
	uint16_t argc = 0;
	char **argv = cmdparse(&argc,cmd);
	if (argv == NULL) return -1;
	uint64_t hash = hashcmd(argv[0]);

	#ifdef DEBUG
	dbgargv(hash,argc);
	dbgtokn(argv);
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
	case EXIT:   free(argv); shsair();       exit(EXIT_SUCCESS);
	default:     status = cmderr(cmd);       break;
	}

	free(argv);
	return status;
}

void prompt(int status) {
	printf("%s:",program_invocation_short_name);
	printf(!gFatplug? CRED"/"CRST:"/");
	printf(BOLD);
	printf(status < 0? CRED"$ "CRST:"$ ");
	printf(NORM);
}

void erro(int errnum) {
	char *app = program_invocation_short_name;
	char *err = strerror(errnum);
	fprintf(stderr,"%s: %s\n",app,err);
}

int cmderr(char *cmd) {
	char *app = program_invocation_short_name;
	char *err = "comando nao encontrado";
	fprintf(stderr,"%s: %s: %s\n",app,err,cmd);
	puts("digite 'help' para ajuda");
	return -1;
}

void shsair(void) {
	puts("Saindo...");
}

void dbgtokn(char **tokens) {
	if (tokens == NULL)
		return;
	int i = 0;
	while (tokens[i] != NULL) {
		printf("tokens[%d] = %s\n",i,tokens[i]);
		i++;
	}
	printf("---------------------\n");
}

void dbgargv(uint64_t hash, uint16_t argc) {
	printf(CYEL);
	printf("  cmdhash-> %ld\n",hash);
	printf("     argc-> %d\n", argc);
	printf(CRST);
}

void eastere(void) {
	uint8_t errcl[] = { 0x4e,0x50,0x2d,0x4a,0x75,0x6e,0x69,0x6e,0x68,0x6f,
	0x3a,0x20,0x50,0x61,0x72,0x61,0x20,0x74,0x6f,0x64,0x6f,0x20,0x70,0x72,
	0x6f,0x62,0x6c,0x65,0x6d,0x61,0x20,0x71,0x75,0x65,0x20,0x6e,0x61,0x6f,
	0x20,0x65,0x78,0x69,0x67,0x65,0x20,0x75,0x6d,0x61,0x20,0x61,0x6c,0x67,
	0x6f,0x72,0x69,0x74,0x6d,0x6f,0x20,0x70,0x6f,0x6c,0x69,0x6e,0x6f,0x6d,
	0x69,0x61,0x6c,0x2c,0x0a,0x65,0x20,0x63,0x72,0x69,0x61,0x64,0x6f,0x20,
	0x75,0x6d,0x20,0x61,0x6c,0x67,0x6f,0x72,0x69,0x74,0x6d,0x6f,0x20,0x70,
	0x6f,0x6c,0x69,0x6e,0x6f,0x6d,0x69,0x61,0x6c,0x2e };
	printf(CYEL"\e[3m%s\e[0m\n"CRST, (char*)errcl);
}