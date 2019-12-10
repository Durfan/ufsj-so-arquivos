#include "main.h"

int init(uint16_t argc) {
	if (argc != 1) {
		erro(EINVAL);
		return -1;
	}
	if (fatexist() && !format())
		return 0;

	FILE *fp = fopen(FATNAME,"wb");
	if (fp == NULL) {
		perror(program_invocation_short_name);
		exit(EXIT_FAILURE);
	}

	memset(gBootblock,0xBB,sizeof(gBootblock));
	fwrite(&gBootblock,sizeof(gBootblock),1,fp);

	gFat[0] = 0xFFFD;
	for (int i=1; i < 9; ++i)
		gFat[i] = 0xFFFE;
	gFat[9] = 0xFFFF;
	for (int i=10; i < NUMCLUSTERS; ++i)
		gFat[i] = 0x0000;
	fwrite(&gFat,sizeof(gFat),1,fp);

	memset(gRootdir,0x00,sizeof(gRootdir));
	fwrite(&gRootdir,sizeof(gRootdir),1,fp);

	for (int i=0; i < 4086; ++i)
		fwrite(&gClusters,sizeof(DataCluster),1,fp);

	fclose(fp);
	gFatplug = true;
	return 0;
}

int load(uint16_t argc) {
	if (argc != 1) {
		erro(EINVAL);
		return -1;
	}

	FILE *fp = fopen(FATNAME,"rb");
	if (fp == NULL) {
		erro(ENODEV);
		return -1;
	}

	fseek(fp,sizeof(gBootblock),SEEK_SET);
	fread(gFat,sizeof(gFat),1,fp);
	fread(gRootdir,sizeof(gRootdir),1,fp);
	fclose(fp);
	gFatplug = true;
	return 0;
}

int ls(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc > 2) {
		erro(EINVAL);
		return -1;
	}
	else if (argc == 1) {
		prtls(rdClster(9),"/",9);
		return 9;
	}

	DataCluster clster;
	DirEntry folder;
	int i=0, block=9;
	int idx    = -1;
	int exists = -1;
	int attrib = -1;

	char *argv1 = strdup(argv[1]);
	char **path = tkenizer(argv[1],"/");

	while (path[i] != NULL) {
		clster = rdClster(block);
		folder = getentry(clster,path[i],&idx);
		exists = folder.firstblock;
		attrib = folder.attributes;

		if (exists == 0) {
			free(argv1);
			free(path);
			erro(ENOENT);
			return -1;
		} else if (attrib == 0) {
			free(argv1);
			free(path);
			erro(ENOTDIR);
			return -1;
		}
		else block = exists;
		i++;
	}

	clster = rdClster(block);
	prtls(clster,argv1,block);

	free(argv1);
	free(path);
	return block;
}

int mkdir(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	DataCluster clster;
	DirEntry folder;
	int i=0, block=9;
	int idx    = -1;
	int exists = -1;
	int attrib = -1;

	char **path = tkenizer(argv[1],"/");
	if (maxdname(path))
		return -1;

	while (path[i] != NULL) {
		clster = rdClster(block);
		folder = getentry(clster,path[i],&idx);
		exists = folder.firstblock;
		attrib = folder.attributes;

		if (exists == 0) {
			folder = newentry(path[i],0x01);
			clster = crtdir(clster,folder);
			wrClster(block,clster);
			writeFAT();
			block = folder.firstblock;
		} else if (attrib != 1) {
			erro(ENOTDIR);
			free(path);
			return -1;
		} else block = exists;
		i++;
	}

	free(path);
	return block;
}

int create(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	DataCluster clster;
	DirEntry file;
	int idx  = -1;
	int block = 9;

	char *argv1 = strdup(argv[1]);
	char *filename = strrchr(argv1,'/');
	char *entry = NULL;

	if (filename == NULL) {
		clster = rdClster(block);
		entry  = argv1;
	}
	else {
		filename[0] = '\0';
		filename += 1;
		block = mkdir(2,mkpath(argv1));
		if (block == -1) {
			free(argv1);
			return -1;
		}
		clster = rdClster(block);
		entry = filename;
	}

	file = getentry(clster,entry,&idx);
	if (file.size != 0x00000000) {
		free(argv1);
		erro(EEXIST);
		return -1;
	} else if (strlen(entry)>17) {
		free(argv1);
		erro(ENAMETOOLONG);
		return -1;
	}

	file = newentry(entry,0x00);
	clster = crtdir(clster,file);
	wrClster(block,clster);
	writeFAT();

	free(argv1);
	return 0;
}

int unlink(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	DirEntry folder;
	DataCluster cluster;
	int idx = -1;
	int i=0, block=9, last = 9;
	int exists = -1;

	char **path = tkenizer(argv[1],"/");

	while (path[i] != NULL) {
		cluster = rdClster(block);
		folder = getentry(cluster,path[i],&idx);
		exists = folder.firstblock;

		if (exists == 0) {
			free(path);
			erro(ENOENT);
			return -1;
		} else {
			last = block;
			block = exists;
		}
		i++;
	}

	if(gFat[(int)folder.firstblock+1] != 0x00) {
		printf("Diretório não vazio.\n");
		return -1;
	}

	i = block;
	if (gFat[i] != 0xFFFF) {
		while (gFat[i] != 0xFFFF) {
			cluster = rdClster((int)gFat[i]-1);
			memset(cluster.data,0x00,sizeof(cluster.data));
			wrClster(i,cluster);
			gFat[i] = 0x00;
			i++;
		}
		cluster = rdClster((int)gFat[i]-1);
		memset(cluster.data, 0x00, sizeof(cluster.data));
		wrClster(i,cluster);
		gFat[i] = 0x00;
	}
	cluster = rdClster(block);
	memset(cluster.dir,0x00,sizeof(cluster.dir));
	gFat[block] = 0x00;

	wrClster(last,cluster);
	writeFAT();

	free(path);
	return 0;
}

int write(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc != 3) {
		erro(EINVAL);
		return -1;
	}

	DataCluster clster;
	DirEntry file;
	int i=0, block=9;
	int idx    = -1;
	int entry  = -1;
	int exists = -1;
	int attrib = -1;

	char **path = tkenizer(argv[2],"/");

	while (path[i] != NULL) {
		clster = rdClster(block);
		file   = getentry(clster,path[i],&idx);
		exists = file.firstblock;
		attrib = file.attributes;

		if (exists == 0) {
			free(path);
			erro(ENOENT);
			return -1;
		}
		else {
			entry = block;
			block = exists;
		}
		i++;
	}

	clster = rdClster(block);
	exists = strlen((char*)clster.data);

	if (attrib != 0) {
		free(path);
		erro(EISDIR);
		return -1;
	} else if (exists != 0) {
		free(path);
		erro(EEXIST);
		return -1;
	}

	int last, blocks = strlen(argv[1]) / CLUSTER;
	if (strlen(argv[1]) % (CLUSTER) != 0) blocks+=1;

	if (blocks > 1) {
		for (int i=0; i < blocks; i++) {
			strncpy((char*)clster.data,&argv[1][i*CLUSTER],CLUSTER);
			wrClster(block,clster);
			last = block;
			gFat[block] = 0xFFFF;
			gFat[block] = freeAddr();
			block = gFat[block];
			clster = rdClster(block);
		}
		gFat[last] = 0xFFFF;
		writeFAT();

		clster = rdClster(entry);
		clster.dir[idx].size = (blocks * CLUSTER);
		wrClster(entry,clster);
	}
	else {
		memcpy(clster.data,argv[1],strlen(argv[1]) * sizeof(char));
		wrClster(block,clster);
	}

	free(path);
	return 0;
}

int append(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc != 3) {
		erro(EINVAL);
		return -1;
	}

	DataCluster clster;
	DirEntry file;
	int i=0, block=9;
	int idx    = -1;
	int entry  = -1;
	int exists = -1;
	int attrib = -1;

	char **path = tkenizer(argv[2],"/");

	while (path[i] != NULL) {
		clster = rdClster(block);
		file   = getentry(clster,path[i],&idx);
		exists = file.firstblock;
		attrib = file.attributes;

		if (exists == 0) {
			free(path);
			erro(ENOENT);
			return -1;
		}
		else {
			entry = block;
			block = exists;
		}
		i++;
	}

	if (attrib != 0) {
		free(path);
		erro(EISDIR);
		return -1;
	}

	int last;
	while (block != 0xFFFF) {
		last = block;
		block = gFat[block];
	}

	block  = last;
	clster = rdClster(last);

	char buffer[8192] = { 0 };
	memcpy(buffer,clster.data,strlen(argv[1]) * sizeof(char));
	strncat(buffer,argv[1],strlen(argv[1]) * sizeof(char));

	int blocks = strlen(buffer) / CLUSTER;
	if (strlen(buffer) % (CLUSTER) != 0) blocks+=1;

	if (blocks > 1) {
		for (int i=0; i < blocks; i++) {
			strncpy((char*)clster.data,&buffer[i*CLUSTER],CLUSTER);
			wrClster(block,clster);
			last = block;
			gFat[block] = 0xFFFF;
			gFat[block] = freeAddr();
			block = gFat[block];
			clster = rdClster(block);
		}
		gFat[last] = 0xFFFF;
		writeFAT();

		clster = rdClster(entry);
		clster.dir[idx].size += ((blocks-1) * CLUSTER);
		wrClster(entry,clster);
	}
	else {
		memset(clster.data,0,CLUSTER * sizeof(char));
		memcpy(clster.data,buffer,strlen(buffer) * sizeof(char));
		wrClster(block,clster);
	}

	free(path);
	return 0;
}

int read(uint16_t argc, char **argv) {
	if (isLoaded() == 0) {
		erro(ENXIO);
		return -1;
	} else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	DataCluster clster;
	DirEntry file;
	int i=0, block=9;
	int idx    = -1;
	int exists = -1;
	int attrib = -1;

	char *argv1 = strdup(argv[1]);
	char **path = tkenizer(argv[1],"/");

	while (path[i] != NULL) {
		clster = rdClster(block);
		file   = getentry(clster,path[i],&idx);
		exists = file.firstblock;
		attrib = file.attributes;

		if (exists == 0) {
			free(argv1);
			free(path);
			erro(ENOENT);
			return -1;
		} else block = exists;
		i++;
	}

	if (attrib != 0) {
		free(path);
		erro(EISDIR);
		return -1;
	}

	int size = 0;
	do {
		clster = rdClster(block);
		printf("%s", clster.data);
		block = gFat[block];
		size += CLUSTER;
	} while (block != 0xFFFF);

	printf("\n"CYEL);
	printf("Arquivo: '%s' Tamanho: %dB",argv1,size);
	printf("\n"CRST);

	free(argv1);
	free(path);
	return 0;
}