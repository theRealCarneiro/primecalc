#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include "paralelo.h"

int* read_file (int* array_length, int work_queue_size) {
	FILE* file;
	*array_length = 0;

	// Get number of lines in file
	int c = 0;
	file = fopen (FILE_PATH, "r");
	while ((c = fgetc (file)) != EOF) {
		if (c == '\n') {
			(*array_length)++;
		}
	}

	fclose (file);

	int* df = (int*) malloc (sizeof (int) * (*array_length));

	// Insert data into array
	file = fopen (FILE_PATH, "r");
	for (int i = 0; fscanf (file, "%d", df + i) == 1; i++);
	fclose (file);

	return df;
}

void write_file (int* buffer, int array_length) {

	FILE* file = fopen (OUT_FILE_PATH, "w");
	for (int i = 0; i < array_length; i++) {
		fprintf (file, "%d", buffer[i]);
		if (i != array_length - 1)
			fprintf (file, "\n");
	}
	fclose (file);
}

int main (int argc, char** argv) {

	struct timeval tstart, tend;
    int process_rank, num_process, task_len, num_workers, work_queue_size, array_length;
	int* df;

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank (MPI_COMM_WORLD, &process_rank);

	if (process_rank == 0) {
		num_workers = num_process - 1;
		work_queue_size = num_workers * 2;
		df = read_file(&array_length, work_queue_size);
		task_len = (int) ((array_length) / work_queue_size);
		printf("NUM WORKERS %d WORK QUEUE LEN %d ARRAY LEN %d TASK LEN %d\n", num_workers, work_queue_size, array_length, task_len);
	}

	// Send task len to clients
	/*MPI_Bcast (&task_len, 2, MPI_INT, 0, MPI_COMM_WORLD);*/

	gettimeofday (&tstart, NULL);

	// Handle worker requests
	if (process_rank == 0) {
		handle_workers (df, work_queue_size, num_workers, array_length, task_len);
	}

	// Request and process tasks
	else {
		handle_tasks(process_rank);
	}

	// Write to outfile
	if (process_rank == 0) {

		/*for (int i = 0; i < array_length - 1; i++)*/
			/*printf("%d\n", df[i]);*/

		gettimeofday (&tend, NULL);

		double time_taken = (tend.tv_sec - tstart.tv_sec) * 1e6;
		time_taken = (time_taken + (tend.tv_usec - 
							  tstart.tv_usec)) * 1e-6;
		printf ("%fs\n", time_taken);

		// Write to file
		write_file(df, array_length);
	}

	free (df);
	MPI_Finalize ();
	return 0;
}
