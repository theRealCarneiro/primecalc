#include <stdio.h>
#include <stdlib.h>
#include "prime.h"

int main() {
	char* file_path = "entrada.txt";

	// Get number of lines in file
	int len, c = 0;
	FILE* file = fopen(file_path, "r");
	while ((c = fgetc(file)) != EOF) {
		if (c == '\n') {
			len++;
		}
    }

	fclose(file);

	int* df = (int*) malloc(sizeof(int) * len);

	// Insert data into array
	file = fopen(file_path, "r");
	for (int i = 0; fscanf(file, "%d", df + i) == 1; i++);
	fclose(file);

	// Calulate primes
	file = fopen("saida.txt", "w");
	int n = 0;
	for (int i = 0; i < len; i++) {
		n = div_num(df[i]);
		fprintf(file, "%d", n);
		if (i != len - 1)
			fprintf(file, "\n");
	}
	fclose(file);

	return 0;
}
