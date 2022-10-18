#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int div_num(int x);

int main(int argc, char** argv) {


	const char* file_path = "entrada.txt";
	FILE* file;
    int process_rank, sclients, len_for_cluster;
	int len = 0;
	int* df;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &sclients);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);


	if (process_rank == 0) {
		char* file_path = "entrada.txt";

		// Get number of lines in file
		int c = 0;
		file = fopen(file_path, "r");
		while ((c = fgetc(file)) != EOF) {
			if (c == '\n') {
				len++;
			}
		}

		fclose(file);

		len_for_cluster = (int) (len / (sclients - 1));

		df = (int*) malloc(sizeof(int) * len);

		// Insert data into array
		file = fopen(file_path, "r");
		for (int i = 0; fscanf(file, "%d", df + i) == 1; i++);
		fclose(file);

	}

	int info[2] = {len, len_for_cluster};
	MPI_Bcast(info, 2, MPI_INT, 0, MPI_COMM_WORLD);

	// Send pieces to clients
	if (!process_rank) {
		for (int i = 1; i < sclients; i++){
			MPI_Send(&df[i * len_for_cluster - len_for_cluster],
					len_for_cluster, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	}

	else {

		// Recive array from master
		int* buffer = malloc(sizeof(int) * info[1]);
		MPI_Recv(buffer, info[1], MPI_INT, 0, 1,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);


		// Calulate divs
		int n = 0;
		for (int i = 0; i < info[1]; i++) {
			buffer[i] = div_num(buffer[i]);
		}

		// send divs back to master
		MPI_Send(buffer, info[1], MPI_INT, 0, 1, MPI_COMM_WORLD);
		free(buffer);
	}

	if (process_rank == 0){

		// Recover divs
		int* buffer = malloc(sizeof(int) * len);
		for (int i = 1; i < sclients; i++){
			MPI_Recv(&buffer[i * len_for_cluster - len_for_cluster],
					len_for_cluster, MPI_INT, i, 1,
					MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// Write to file
		file = fopen("saida.txt", "w");
		for (int i = 0; i < len; i++) {
			fprintf(file, "%d", buffer[i]);
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
	int div = x == 1 ? 1 : 2;

	// odd numbers are never divisible by even numbers, so we skip them
	int step = x % 2 == 0 ? 1 : 2;

	// min possible div for even numbers is 2 and for odd numbers is 3
	int min_pos_div = step + 1;

	// max possible div is x / min_div (besides itself)
	int max_pos_div = (int)(x / min_pos_div);

	// Calculate actual min and max div
	int min_div, max_div;
	for (min_div = min_pos_div;
			x % min_div != 0 && min_div == max_pos_div;
			min_div += step);
	max_div = x / min_div;

	// if the number is prime, it won't even enter the loop
	for (int i = min_div; i <= max_div; i += step) {
		if (x % i == 0)
			div++;
	}

	return div;
}
