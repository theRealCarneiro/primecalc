#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int div_num(int x);

int main(int argc, char** argv) {
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


    int process_rank, scluster;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &scluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	int len_for_cluster = (int) (len / (scluster - 1));

	if (process_rank == 0) {
		for (int i = 1; i < scluster; i++){
			MPI_Send(&df[i * len_for_cluster - len_for_cluster], len_for_cluster, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	}
	else {
		int* message_item = (int*) malloc(sizeof(int) * len_for_cluster);
		MPI_Recv(message_item, len_for_cluster, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Calulate divs
		int n = 0;
		for (int i = 0; i < len_for_cluster; i++) {
			df[i] = div_num(df[i]);
		}

		// send divs back to master
		MPI_Send(df, len_for_cluster, MPI_INT, 0, 1, MPI_COMM_WORLD);
		free(message_item);
	}

	if (process_rank == 0){

		// Recover divs
		for (int i = 1; i < scluster; i++){
			MPI_Recv(&df[i * len_for_cluster - len_for_cluster], len_for_cluster, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// Write to file
		file = fopen("saida.txt", "w");
		for (int i = 0; i < len; i++) {
			fprintf(file, "%d", df[i]);
			if (i != len - 1)
				fprintf(file, "\n");
		}
		fclose(file);

	}

	MPI_Finalize();
	free(df);

	return 0;
}

int div_num(int x) {

	// every number is divisible by 1 and itself, so we can skip them
	int div = 2;

	// odd numbers are never divisible by even numbers, so we skip them
	int step = x % 2 == 0 ? 1 : 2;

	// min possible div for even numbers is 2 and for odd numbers is 3 (besides 1)
	int min_div = step + 1;

	// max possible div is x / min_div (besides itself)
	for (int i = min_div; i < (int)(x / min_div); i += step) {
		if (x % i == 0)
			div++;
	}

	return div;
}
