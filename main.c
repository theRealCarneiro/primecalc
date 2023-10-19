#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include "paralelo.h"

#define WORK_QUEUE_FACTOR 10

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

double calculate_time(struct timeval tstart, struct timeval tend) {
	double time_taken = (tend.tv_sec - tstart.tv_sec) * 1e6;
	time_taken = (time_taken + (tend.tv_usec - 
							  tstart.tv_usec)) * 1e-6;
	return time_taken;
}

int main (int argc, char** argv) {

	struct timeval tstart, tend;
    int process_rank, num_process, task_len, num_workers, work_queue_size, array_length;
	int* df;

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank (MPI_COMM_WORLD, &process_rank);

	if (process_rank == 0) {

		// Get number of workers, tasks and the array
		num_workers = num_process - 1;
		work_queue_size = num_workers * WORK_QUEUE_FACTOR;
		df = read_file(&array_length, work_queue_size);

		// If there's still some work left, up the number of tasks
		if (array_length % work_queue_size != 0) {
			work_queue_size++;
		}

		task_len = (int) ((array_length) / work_queue_size);

		printf("NUM WORKERS %d WORK QUEUE LEN %d ARRAY LEN %d TASK LEN %d\n", num_workers, work_queue_size, array_length, task_len);

		gettimeofday (&tstart, NULL);
		handle_workers (df, work_queue_size, num_workers, array_length, task_len);
		gettimeofday (&tend, NULL);

		write_file(df, array_length);
	}

	// Request and process tasks
	else {
		gettimeofday (&tstart, NULL);
		handle_tasks(process_rank);
		gettimeofday (&tend, NULL);
	}

	double time_taken = calculate_time(tstart, tend);
	printf ("RANK %d %fs\n", process_rank, time_taken);

	free (df);
	MPI_Finalize ();
	return 0;
}
