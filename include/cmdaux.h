#ifndef INCLUDE_CMDAUX_H
#define INCLUDE_CMDAUX_H

DataCluster crtdir(DataCluster cluster, DirEntry folder);
DirEntry newentry(char *filename, uint8_t attr);
DirEntry getentry(DataCluster cluster, char *path);
void prtls(DataCluster cluster, char *path, int block);

int maxdname(char **path);
int fatexist(void);
int   format(void);
void clrBuff(void);
void    help(void);

bool isLoaded(void);

//#pragma message __FILE__
#endif // INCLUDE_CMDAUX_H