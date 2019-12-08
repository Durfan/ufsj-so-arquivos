#include "main.h"

int init(uint16_t argc) {
	if (argc != 1) {
		erro(EINVAL);
		return -1;
	}

	if (fatexist() && format() == 0)
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
	if (fatplug())
		return -1;
	else if (argc > 2) {
		erro(EINVAL);
		return -1;
	}

	if (argc == 1) {
		prtls(readCL(9),9,"/");
		return 9;
	}

	DataCluster cluster;
	int i=0,exists,block=9,tks=0;

	char *delim = "/";
	char *argv1 = strdup(argv[1]);
	char **path = tkenizer(argv[1],delim,&tks);

	while (path[i] != NULL) {
		cluster = readCL(block);
		exists  = dirSET(cluster,path[i]);

		if (exists == -1) {
			free(argv1);
			free(path);
			erro(ENOENT);
			return -1;
		} else if (exists == -2) {
			free(argv1);
			free(path);
			erro(ENOTDIR);
			return -1;
		}
		else block = exists;
		i++;
	};

	cluster = readCL(block);
	prtls(cluster,block,argv1);

	free(argv1);
	free(path);
	return block;
}

int mkdir(uint16_t argc, char **argv) {
	if (fatplug())
		return -1;
	else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	DataCluster cluster;
	DirEntry folder;
	int i=0,exists,block=9,tks=0;

	char *delim = "/";
	char **path = tkenizer(argv[1],delim,&tks);
	if (maxdname(path))
		return -1;

	while (path[i] != NULL) {
		cluster = readCL(block);
		exists  = dirSET(cluster,path[i]);

		if (exists == -1) {
			folder  = newentry(path[i],0x01);
			cluster = crtdir(cluster,folder);
			writeCL(block,cluster);
			writeFAT();
			block = folder.firstblock;
		} else if (exists == -2) {
			erro(ENOTDIR);
			free(path);
			return -1;
		} else block = exists;
		i++;
	};

	free(path);
	return block;
}

int create(uint16_t argc, char **argv) {
	if (fatplug())
		return -1;
	else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	char *argv1 = strdup(argv[1]);
	char *filename = strrchr(argv1,'/');
	char *entry;
	int exists, block = 9;
	DataCluster cluster;
	DirEntry file;

	if (filename == NULL) {
		cluster = readCL(block);
		entry = argv1;
		exists = dirSET(cluster,entry);
	}
	else {
		filename[0] = '\0';
		filename += 1;
		block = mkdir(2,mkpath(argv1));
		if (block == -1) {
			free(argv1);
			return -1;
		}
		cluster = readCL(block);
		entry = filename;
		exists = dirSET(cluster,entry);
	}

	if (exists == -2) {
		erro(EEXIST);
		free(argv1);
		return -1;
	}

	file = newentry(entry,0x00);
	cluster = crtdir(cluster,file);
	writeCL(block,cluster);
	writeFAT();

	free(argv1);
	return 0;
}

/* int unlink(uint16_t argc, char **argv) {

	return 0;
} */

int write(uint16_t argc, char **argv) {
	if (fatplug())
		return -1;
	else if (argc != 3) {
		erro(EINVAL);
		return -1;
	}

	char *argv2 = strdup(argv[2]);
	int   block = ls(2,lspath(argv2));
	int  blocks = (strlen(argv[1]) / (CLUSTER)) + 1;

	DataCluster cluster = readCL(block);

	for (int i=0; i < blocks; i++) {
		memcpy(cluster.data,argv[1],strlen(argv[1]) * sizeof(char));
		writeCL(block,cluster);
		block = freeAddr();
	}
	gFat[block] = 0xFFFF;

	free(argv2);
	return 0;
}

/*

int append(uint16_t argc, char **argv) {

	return 0;
}

int read(uint16_t argc, char **argv) {

	return 0;
}
*/