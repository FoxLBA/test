//#include <my_global.h>
//#include <mysql.h>
#include <stdio.h>
#include <string.h>

char par1[255]="Wlen=690.0&Psize=1.562&Imgsize=1024&X0=0&and&then&was&silence";
char par2[255]="ThProbability=0.020000&ThAmp=200.000000&MinObjSize=0&MaxObjSize=16&RandomShit=0.666";

int main() {
	int i=0;
	char filename[255];
	FILE *config;
	printf("hello world!\n");
	config = fopen("config.txt", "w");
	if (config==NULL)
	{
		printf("error\n");
	}
	//parsing
	
	while (par1[i]!='\0' || par2[i]!='\0') {
		if (par1[i]=='&') { 
			par1[i]='\n';
		}
		if (par2[i]=='&') { 
			par2[i]='\n';
		}
		i++;
	}
//	while (par2[i]!='\0') {
//		if (par2[i]=='&') { 
//			par2[i]='\n';
//		}
//		i++;
//	}
	//write to config.txt
	fprintf(config, "Filename=%s\n[Main parameters]\n%s\n\n[Search parameters]\n%s", filename, par1, par2);
}
