#ifndef INCLUDE_FAT_H
#define INCLUDE_FAT_H

#define SECTORS 512
#define CLUSTER 2*SECTORS
#define ENTRYBYCLUSTER CLUSTER/sizeof(DirEntry)
#define NUMCLUSTERS 4096
#define FATNAME "fat.part"

/* entrada de diretorio, 32 bytes cada */
typedef struct direntry_t {
	uint8_t  filename[18];	// 18 bytes
	uint8_t  attributes;	//  1 byte, valor: 0 arquivo, 1 diretorio
	uint8_t  reserved[7];	//  7 bytes
	uint16_t firstblock;	//  2 bytes
	uint32_t size;			//  4 bytes
} DirEntry;

/* diretorios (incluindo ROOT),
   32 entradas de diretorio com 32 bytes cada = 1024 bytes
   ou bloco de dados de 1024 bytes */
typedef union datacluster_t {
	DirEntry dir[CLUSTER / sizeof(DirEntry)];
	uint8_t data[CLUSTER];
} DataCluster;

uint16_t gFat[NUMCLUSTERS]; // 8 clusters da tabela FAT, 4096 entradas de 16 bits = 8192 bytes
uint8_t  gBootblock[CLUSTER];
DirEntry gRootdir[32];
DataCluster gClusters[4086];
bool gFatplug;

DataCluster readCL(int index);
void  writeCL(int index, DataCluster cluster);
void writeFAT(void);
int findSpace(void);

//#pragma message __FILE__
#endif // INCLUDE_FAT_H