#ifndef INCLUDE_CMDAUX_H
#define INCLUDE_CMDAUX_H

DataCluster crtdir(DataCluster cluster, DirEntry folder);
DirEntry newentry(char *filename, uint8_t attr);
int dirSET(DataCluster cluster, char *path, bool file);
void prtls(DataCluster cluster, char *path, int block);

int maxdname(char **path);
int fatexist(void);
int fatplug(void);
int  format(void);
void  clrBf(void);
void   help(void);

//#pragma message __FILE__
#endif // INCLUDE_CMDAUX_H