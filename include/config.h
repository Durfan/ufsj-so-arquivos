#ifndef INCLUDE_CONFIG_H
#define INCLUDE_CONFIG_H

extern char *program_invocation_name;
extern char *program_invocation_short_name;
#define VERSION "0.0.1"
#define TAG "alpha"

#define CRED  "\x1b[31m"
#define CYELL "\x1b[33m"
#define CRSET "\x1b[0m"

#define SECTOR_SIZE 512
#define CLUSTERSIZE 2*SECTOR_SIZE
#define ENTRYBYCLUSTER CLUSTERSIZE/sizeof(DirEntry)
#define NUMCLUSTER 4096
#define FATNAME "fat.part"

typedef struct direntry_t {
	unsigned char filename[18];
	unsigned char attributes;
	unsigned char rserved[7];
	unsigned short firstblock;
	unsigned int size;
} DirEntry;

typedef union datacluster_t {
	DirEntry dir[CLUSTERSIZE / sizeof(DirEntry)];
	DirEntry data;
} DataCluster;

unsigned short g_fat[NUMCLUSTER];
unsigned char g_bootblock[CLUSTERSIZE];
DirEntry g_rootdir[32];
DataCluster g_clusters[4086];

//#pragma message __FILE__
#endif // INCLUDE_CONFIG_H