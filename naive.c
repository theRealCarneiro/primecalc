#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include "prime.c"

int main(int argc, char** argv) {


	struct timeval tstart, tend;
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

	gettimeofday(&tstart, NULL);

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
		# pragma omp paralell for schedule(dynamic)
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
		gettimeofday(&tend, NULL);

		double time_taken = (tend.tv_sec - tstart.tv_sec) * 1e6;
		time_taken = (time_taken + (tend.tv_usec - 
							  tstart.tv_usec)) * 1e-6;
		printf("%fs\n", time_taken);

		// Write to file
		file = fopen("saida.txt", "w");
		for (int i = 0; i < len; i++) {
			fprintf(file, "%d", buffer[i]);
			if (i != len - 1)
				fprintf(file, "\n");
		}
		fclose(file);
		free(df);
	}

	MPI_Finalize();

	return 0;
}
