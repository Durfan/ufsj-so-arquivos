#ifndef INCLUDE_SHELL_H
#define INCLUDE_SHELL_H

#define MAXCMD 64
#define freebuf(buff) \
	memset(buff,'\0',MAXCMD * sizeof(char));

#define INIT	6385337657
#define LOAD	6385446277
#define LS		5863588
#define MKDIR	210720772860
#define CREATE	6953402479289
#define UNLINK	6954102432758
#define WRITE	210732889424
#define APPEND	6953322236125
#define READ	6385651009
#define HELP	6385292014
#define EXIT	6385204799

unsigned long hashcmd(char *cmd);
int init(void);
int load(void);
void shell(void);
char **tkenizer(char *input);
int commands(char *cmd);
void prompt(int status);
void cmderr(char *cmd);
void shsair(void);
void help(void);

//#pragma message __FILE__
#endif // INCLUDE_SHELL_H