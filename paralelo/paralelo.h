#ifndef PARALELO_H
#define PARALELO_H

#define REQUEST_TASK 0
#define TASK_DONE 1
#define NO_MORE_TASKS -1

#define REQUEST 1
#define SEND_TASK 2
#define RETRIEVE_TASK 3

#define MASTER_RANK 0

#define FILE_PATH "entrada.txt"
#define OUT_FILE_PATH "saida.txt"


typedef struct slice {
	int* array;	
	int index;
	int len;
} slice;


void handle_workers (int* df, int work_queue_size, int num_workers, int array_length, int task_len);
void retrieve_task (int* df, int worker_rank);
int send_task (int* df, int task_len, int task_index, int worker_rank,
				int work_queue_size, int array_length);

slice request_task (int process_rank);
void send_task_done (int* task, int task_len, int task_index, int process_rank);
void handle_tasks (int process_rank);
#endif /* PARALELO_H */
