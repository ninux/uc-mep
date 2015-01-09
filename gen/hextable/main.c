#include <stdio.h>

int main(char argc, char** argv)
{
	FILE* file;
	int i, limit;
	char* filename;

	i = 0;
	limit = 0xFF;

	filename = "hextable.data";

	file = fopen(filename, "w");

	for (i = 0; i < limit; i++) {
		fprintf(file, "%i\t& %X\n", i, i);
	}

	fclose(file);

	return 0;
}
