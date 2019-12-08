#ifndef INCLUDE_CMDAUX_H
#define INCLUDE_CMDAUX_H

DataCluster crtdir(DataCluster cluster, DirEntry folder);
DirEntry newentry(char *filename, uint8_t attr);
DirEntry getentry(DataCluster cluster, int index);
int dirSET(DataCluster cluster, char *path);
void prtls(DataCluster cluster, int block, char *path);

int maxdname(char **path);
int fatexist(void);
int fatplug(void);
int  format(void);
void  clrBf(void);
void   help(void);

//#pragma message __FILE__
#endif // INCLUDE_CMDAUX_H