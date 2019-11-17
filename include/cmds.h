#ifndef INCLUDE_CMDS_H
#define INCLUDE_CMDS_H

int init(uint16_t argc);
int load(uint16_t argc);
int ls(uint16_t argc, char **argv);
int mkdir(uint16_t argc, char **argv);
int create(void);
int unlink(void);
int write(void);
int append(void);
int read(void);
void help(void);

DataCluster *readCL(int index);
void writeCL(int index, DataCluster *cluster);
int findSpace(void);

//#pragma message __FILE__
#endif // INCLUDE_CMDS_H