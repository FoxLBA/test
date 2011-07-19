//#include <my_global.h>
//#include <mysql.h>
#include <stdio.h>

char par1[255]="Wlen=690.0&Psize=1.562&Imgsize=1024&X0=0";
char par2[255]="ThProbability=0.020000&ThAmp=200.000000&MinObjSize=0&MaxObjSize=16";
char par3[255]="Wlen=690.0\nPsize=1.562\nImgsize=1024&X0=0";

int main() {
	char filename[255];
	FILE *config;
	printf("hello world!\n");
	config = fopen("config.txt", "w");
	if (config==NULL)
	{
		printf("error\n");
	}
	fprintf(config, "%s", par3);
}
