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
		char *app = program_invocation_short_name;
		char *err = strerror(ENODEV);
		fprintf(stderr,"%s: %s\n",app,err);
		puts("digite 'help' para ajuda");
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
	if (gFatplug == false) {
		erro(ENXIO);
		return -1;
	} else if (argc > 2) {
		erro(EINVAL);
		return -1;
	}

	if (argc == 1) {
		prtls(readCL(9),9,"root");
		return 9;
	}

	DataCluster cluster;
	int i=0,exists,block=9,tks=0;

	char *delim = "/";
	char **path = tkenizer(argv[1],delim,&tks);
	char *argv1 = strdup(argv[1]);

	do {
		cluster = readCL(block);
		exists  = dirSET(cluster,path[i]);

		if (exists < 0) {
			free(argv1);
			free(path);
			erro(ENOENT);
			return -1;
		}
		else block = exists;

	} while (path[++i] != NULL);

	cluster = readCL(block);
	prtls(cluster,block,argv1);

	free(argv1);
	free(path);
	return block;
}

int mkdir(uint16_t argc, char **argv) {
	if (gFatplug == false) {
		erro(ENXIO);
		return -1;
	} else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	DataCluster cluster;
	DirEntry folder;
	int i=0,exists,block=9,tks=0;

	char *delim = "/";
	char **path = tkenizer(argv[1],delim,&tks);

	while (path[i] != NULL) {
		cluster = readCL(block);
		exists  = dirSET(cluster,path[i]);

		if (exists < 0) {
			folder  = newentry(path[i],0x01);
			cluster = crtdir(cluster,folder);
			writeCL(block,cluster);
			writeFAT();
			block = folder.firstblock;
		}
		else block = exists;
		i++;
	};

	free(path);
	return block;
}

int create(uint16_t argc, char **argv) {
	if (gFatplug == false) {
		erro(ENXIO);
		return -1;
	} else if (argc != 2) {
		erro(EINVAL);
		return -1;
	}

	char *argv1 = strdup(argv[1]);
	char *filename = strrchr(argv1,'/');
	if (filename == NULL) {
		free(argv1);
		return -1;
	}

	filename[0] = '\0';
	filename += 1;

	int block = mkdir(2,mkpath(argv1));
	DataCluster cluster = readCL(block);
	DirEntry file = newentry(filename,0x00);

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
	if (gFatplug == false) {
		erro(ENXIO);
		return -1;
	} else if (argc != 3) {
		erro(EINVAL);
		return -1;
	}

	char *argv2 = strdup(argv[2]);
	int block = ls(2,lspath(argv2));

	DataCluster cluster = readCL(block);
	memcpy(cluster.data,argv[1],strlen(argv[1]) * sizeof(char));
	writeCL(block,cluster);

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