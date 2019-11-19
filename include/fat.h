#ifndef INCLUDE_FAT_H
#define INCLUDE_FAT_H

#define SECTORSIZE  512
#define CLUSTERSIZE 2*SECTORSIZE
#define ENTRYBYCLUSTER CLUSTERSIZE/sizeof(DirEntry)
#define NUMCLUSTER  4096
#define FATNAME "fat.part"

/* entrada de diretorio, 32 bytes cada */
typedef struct direntry_t {
	uint8_t  filename[18];	// 18 bytes
	uint8_t  attributes;	//  1 byte, valor: 0 arquivo, 1 diretorio
	uint8_t  reserved[7];	//  7 bytes
	uint16_t firstblock;	//  2 bytes
	uint32_t size;			//  4 bytes
} DirEntry;

/*	diretorios (incluindo ROOT),
	32 entradas de diretorio com 32 bytes cada = 1024 bytes
	ou bloco de dados de 1024 bytes */
typedef union datacluster_t {
	DirEntry dir[CLUSTERSIZE / sizeof(DirEntry)];
	uint8_t data[CLUSTERSIZE];
} DataCluster;

/* 8 clusters da tabela FAT, 4096 entradas de 16 bits = 8192 bytes */
uint16_t g_fat[NUMCLUSTER];

uint8_t  g_bootblock[CLUSTERSIZE];
DirEntry g_rootdir[32];
DataCluster g_clusters[4086];

DataCluster *readCL(int index);
void  writeCL(int index, DataCluster *cluster);
void writeFAT(void);
int findSpace(void);

//#pragma message __FILE__
#endif // INCLUDE_FAT_H