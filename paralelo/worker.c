#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "paralelo.h"
#include "../prime.c"

void handle_tasks (int process_rank) {

	while (1) {
		printf("WORKER: ");
		slice s = request_task(process_rank);

		// No more tasks
		if (s.index == -1){
			break;
		}

		// Calulate divs
		for (int i = 0; i < s.len; i++) {
			int tmp = s.array[i];
			s.array[i] = div_num (s.array[i]);
		}

		// Send the results back to the master
		send_task_done(s.array, s.len, s.index, process_rank);
		/*free(s.array);*/
	}
}

slice request_task (int process_rank) {

	MPI_Status status;

	int request_type = REQUEST_TASK;
	int task_index, task_len;

	// Request a task from the master
	MPI_Send (&process_rank, 1, MPI_INT, MASTER_RANK, REQUEST, MPI_COMM_WORLD);
	MPI_Send (&request_type, 1, MPI_INT, MASTER_RANK, REQUEST, MPI_COMM_WORLD);

	// Get task index
	MPI_Recv (&task_index, 1, MPI_INT, MASTER_RANK, SEND_TASK, MPI_COMM_WORLD, &status);

	// Create Slice
	slice s = {NULL, task_index, 0};

	// Request task if there's work to be done 
	if (task_index != NO_MORE_TASKS) {

		// Get task len
		MPI_Recv (&task_len, 1, MPI_INT, MASTER_RANK, SEND_TASK, MPI_COMM_WORLD, &status);

		int* buffer = malloc (sizeof (int) * (task_len));

		// Get task
		MPI_Recv (buffer, task_len, MPI_INT, MASTER_RANK, SEND_TASK, MPI_COMM_WORLD, &status);

		// Point slice array to buffer
		s.array = buffer;
		s.len = task_len;
	}
	printf("WORKER %d RECEIVED TASK %d LEN %d\n", process_rank, task_index, task_len);

	return s;
}

void send_task_done (int* task, int task_len, int task_index, int process_rank){

	MPI_Status status;

	int request_type = TASK_DONE;

	MPI_Send (&process_rank, 1, MPI_INT, MASTER_RANK, REQUEST, MPI_COMM_WORLD);
	MPI_Send (&request_type, 1, MPI_INT, MASTER_RANK, REQUEST, MPI_COMM_WORLD);

	MPI_Send (&task_index, 1, MPI_INT, MASTER_RANK, RETRIEVE_TASK, MPI_COMM_WORLD);
	MPI_Send (&task_len, 1, MPI_INT, MASTER_RANK, RETRIEVE_TASK, MPI_COMM_WORLD);
	MPI_Send (task, task_len, MPI_INT, MASTER_RANK, RETRIEVE_TASK, MPI_COMM_WORLD);
	printf("WORKER %d FINISH TASK %d\n", process_rank, task_index);
}
