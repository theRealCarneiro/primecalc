#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "paralelo.h"

void handle_workers (int* df, int work_queue_size, int num_workers, int array_length, int task_len) {

	int worker_rank, request_type;
	int task_index = 0, retrieved_count = 0, clients_done_count = 0;

	while (1) {
		/*printf("\nMASTER: ");*/

		// Retrieved everything, tell clients it' over
		if (retrieved_count >= work_queue_size && clients_done_count >= num_workers) {
			/*printf("ACABO\n");*/
			break;
		}

		MPI_Status status;

		// Receive request from a worker
		MPI_Recv (&worker_rank, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST, MPI_COMM_WORLD, &status);
		/*printf("WORKER %d ", worker_rank);*/
		MPI_Recv (&request_type, 1, MPI_INT, worker_rank, REQUEST, MPI_COMM_WORLD, &status);
		/*printf("REQUEST %d ", request_type);*/

		switch(request_type) {

			case REQUEST_TASK:
				if (send_task(df, task_len, task_index, worker_rank, work_queue_size, array_length))
					task_index++;
				else
					clients_done_count++;

				break;

			case TASK_DONE:
				retrieve_task(df, worker_rank);
				retrieved_count++;
				break;

			default:
				break;
		}


	}
}

int send_task (int* df, int task_len, int task_index, int worker_rank,
				int work_queue_size, int array_length) {


	// if there are no more tasks left
	if (task_index >= work_queue_size) {
		task_index = NO_MORE_TASKS;
		MPI_Send (&task_index, 1, MPI_INT, worker_rank, SEND_TASK, MPI_COMM_WORLD);
		/*printf("NO MORE TASKS\n");*/
		return 0;
	}

	int* task = &df[task_index * task_len];
	
	// make sure we send everything, even when it's an odd number
	if (task_index * task_len + task_len > array_length){
		task_len = array_length - task_index * task_len;
	}

	MPI_Send (&task_index, 1, MPI_INT, worker_rank, SEND_TASK, MPI_COMM_WORLD);
	MPI_Send (&task_len, 1, MPI_INT, worker_rank, SEND_TASK, MPI_COMM_WORLD);
	/*printf("SENDING TASK %d LEN %d\n", task_index, task_len);*/
	MPI_Send (task, task_len, MPI_INT, worker_rank, SEND_TASK, MPI_COMM_WORLD);
	fflush(stdout);
	return 1;
}

void retrieve_task (int* df, int worker_rank) {
	MPI_Status status;
	int task_index, task_len;

	MPI_Recv (&task_index, 1, MPI_INT, worker_rank, RETRIEVE_TASK, MPI_COMM_WORLD, &status);
	MPI_Recv (&task_len, 1, MPI_INT, worker_rank, RETRIEVE_TASK, MPI_COMM_WORLD, &status);
	MPI_Recv (&df[task_index * task_len], task_len, MPI_INT, worker_rank, RETRIEVE_TASK, MPI_COMM_WORLD, &status);
	/*printf("RETRIEVE TASK %d SIZE %d\n", task_index, task_len);*/
}
