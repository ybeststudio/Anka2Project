#include <stdio.h>
#include <stdlib.h>

void WriteVersion()
{
	FILE* fp(fopen("version.txt", "w"));

	if (NULL != fp)
	{
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=      All rights are reserved by [BestStudio]  =|\n");
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "                                                   \n");
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=   DB developed by [BestStudio] version: %s   =|\n", __DB_VERSION__);
		fprintf(fp, "|=                                               =|\n");
		fprintf(fp, "|=================================================|\n");
		fprintf(fp, "                                                   \n");
		fprintf(fp, "%s@%s:%s\n", __USER__, __HOSTNAME__, __PWD__);
		fclose(fp);
	}
	else
	{
		fprintf(stderr, "cannot open version.txt\n");
		exit(0);
	}
}
