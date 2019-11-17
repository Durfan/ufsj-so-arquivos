#ifndef INCLUDE_CONFIG_H
#define INCLUDE_CONFIG_H

extern char *program_invocation_name;
extern char *program_invocation_short_name;
#define VERSION "0.0.2"
#define TAG "alpha"

#define CRED "\x1b[31m"
#define CYEL "\x1b[33m"
#define CRST "\x1b[0m"
#define BOLD "\033[1m"
#define NORM "\033[22m"

#define SECTORSIZE  512
#define CLUSTERSIZE 2*SECTORSIZE
#define ENTRYBYCLUSTER CLUSTERSIZE/sizeof(DirEntry)
#define NUMCLUSTER  4096
#define FATNAME "fat.part"

/* entrada de diretorio, 32 bytes cada */
typedef struct direntry_t {
	uint8_t  filename[18];
	uint8_t  attributes; // valor: 0 arquivo, 1 diretorio
	uint8_t  reserved[7];
	uint16_t firstblock;
	uint32_t size;
} DirEntry;

/* diretorios (incluindo ROOT),
	32 entradas de diretorio com 32 bytes cada = 1024 bytes
	ou bloco de dados de 1024 bytes */
typedef union datacluster_t {
	DirEntry dir[CLUSTERSIZE / sizeof(DirEntry)];
	DirEntry data;
} DataCluster;

/* 8 clusters da tabela FAT, 4096 entradas de 16 bits = 8192 bytes */
uint16_t g_fat[NUMCLUSTER];
uint8_t  g_bootblock[CLUSTERSIZE];
DirEntry g_rootdir[32];
DataCluster g_clusters[4086];

//#pragma message __FILE__
#endif // INCLUDE_CONFIG_H