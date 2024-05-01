#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define NUM_JOBS 100
#define NUM_MACHINES 100

typedef struct {
    int machine;
    int time;
} Job;

typedef struct {
    int job;
    int machine;
    int start_time;
    int end_time;
} Schedule;

typedef struct {
    int start_index;
    int end_index;
} ThreadArgs;

int max(int a, int b) {
    return (a > b) ? a : b;
}

Job jobs[NUM_JOBS][NUM_MACHINES];
int num_jobs, num_machines;
Schedule schedule[NUM_JOBS * NUM_MACHINES];

void *scheduleJobs(void *thread_args) {
    ThreadArgs *args = (ThreadArgs *)thread_args;
    int start_index = args->start_index;
    int end_index = args->end_index;

    int machine_availability[NUM_MACHINES] = {0};
    int job_end_times[NUM_JOBS] = {0};

    for (int i = start_index; i < end_index; i++) {
        for (int j = 0; j < num_machines; j++) {
            int machine = jobs[i][j].machine;
            int start_time = max(machine_availability[machine], job_end_times[i]);

            schedule[i * num_machines + j].job = i;
            schedule[i * num_machines + j].machine = machine;
            schedule[i * num_machines + j].start_time = start_time;
            schedule[i * num_machines + j].end_time = start_time + jobs[i][j].time;

            machine_availability[machine] = schedule[i * num_machines + j].end_time;
            job_end_times[i] = schedule[i * num_machines + j].end_time;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <input_file> <output_file> <num_threads>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    int num_threads = atoi(argv[3]);

    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    // Read job data from file
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Could not open file\n");
        return 1;
    }

    fscanf(file, "%d %d", &num_jobs, &num_machines);

    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines; j++) {
            fscanf(file, "%d %d", &jobs[i][j].machine, &jobs[i][j].time);
        }
    }

    fclose(file);

    clock_t start = clock();

    int jobs_per_thread = num_jobs / num_threads;
    int remainder = num_jobs % num_threads;

    int index = 0;
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].start_index = index;
        thread_args[i].end_index = index + jobs_per_thread + (i < remainder ? 1 : 0);
        pthread_create(&threads[i], NULL, scheduleJobs, (void *)&thread_args[i]);
        index = thread_args[i].end_index;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;

    // Write schedule to output file
    FILE *output = fopen(output_file, "w");
    if (output == NULL) {
        printf("Could not open or create output file\n");
        return 1;
    }

    for (int i = 0; i < num_jobs * num_machines; i++) {
        fprintf(output, "Job %d - Machine %d - Start %d - End %d\n", schedule[i].job, schedule[i].machine, schedule[i].start_time, schedule[i].end_time);
    }

    // Initialize makespan
    int makespan = 0;

    // Calculate makespan
    for (int i = 0; i < num_jobs * num_machines; i++) {
        if (schedule[i].end_time > makespan) {
            makespan = schedule[i].end_time;
        }
    }
    
    fprintf(output, "The makespan is %d\n", makespan);
    fprintf(output, "Time taken: %f seconds\n", time_taken);

    fclose(output);

    return 0;
}
