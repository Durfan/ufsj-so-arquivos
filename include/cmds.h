#ifndef INCLUDE_CMDS_H
#define INCLUDE_CMDS_H

int init(uint16_t argc);
int load(uint16_t argc);
int ls(void);
int create(void);
int unlink(void);
int write(void);
int append(void);
int read(void);
void help(void);

//#pragma message __FILE__
#endif // INCLUDE_CMDS_H