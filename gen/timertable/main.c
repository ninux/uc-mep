#include <stdio.h>
#include <stdlib.h>

#define WRITE "w"

int main(char argc, char** argv)
{
	double clk[] = {24000000, 1000000};
	double tpm[] = {1,2,4,8,16,32,64,128};
	char filename[] = "timetable.txt";

	FILE* file;

	int i;
	int j;


	file = fopen(filename, WRITE);

	for (i = 0; i < (sizeof(clk)/sizeof(clk[0])); i++) {
		for (j = 0; j < (sizeof(tpm)/sizeof(tpm[0])); j++) {
			fprintf(file, "%.0f MHz & %.0f & %f $\\mu$s & %f $\\mu$s \\\\ \n",
				clk[i]/1000000, tpm[j], (1/clk[i])*tpm[j]*1000000,
				(1/clk[i])*tpm[j]*(0xFFFF+1)*1000000);
		}
	}

	fclose(file);

	return 0;
}
