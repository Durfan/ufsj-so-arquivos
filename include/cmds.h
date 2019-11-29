#ifndef INCLUDE_CMDS_H
#define INCLUDE_CMDS_H

int   init(uint16_t argc);
int   load(uint16_t argc);
int     ls(uint16_t argc, char **argv);
int  mkdir(uint16_t argc, char **argv);
int create(uint16_t argc, char **argv);
int unlink(uint16_t argc, char **argv);
int  write(uint16_t argc, char **argv);
int append(uint16_t argc, char **argv);
int   read(uint16_t argc, char **argv);
void  help(void);

DirEntry newdir(char *filename);

//#pragma message __FILE__
#endif // INCLUDE_CMDS_H