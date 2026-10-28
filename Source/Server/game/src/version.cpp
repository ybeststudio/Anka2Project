#include <stdio.h>

void WriteVersion()
{
	FILE* fp = fopen("version.txt", "w");

	if (fp)
	{
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=      All rights are reserved by [BestStudio]  =|\n");
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "                                                   \n");
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=  Game developed by [BestStudio] version: %s  =|\n", __GAME_VERSION__);
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "                                                   \n");
		fprintf(fp, "%s@%s:%s\n", __USER__, __HOSTNAME__, __PWD__);
		fclose(fp);
	}
}
