#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>

// Maximum limits for jobs, machines, and time slots
#define MAX_JOBS 100
#define MAX_MACHINES 100
#define MAX_TIME 1000000

// Structure to represent a job operation (machine and processing time)
typedef struct
{
    int machine;
    int time;
} Job;

// Structure to represent a scheduled operation
typedef struct
{
    int job;
    int machine;
    int start_time;
    int end_time;
} Schedule;

// Structure to pass arguments to threads
typedef struct
{
    int start_job_index;
    int end_job_index;
} ThreadArgs;

// Global variables and data structures
struct timeval start, end;                     // For timing the execution
Job jobs[MAX_JOBS][MAX_MACHINES];              // Job operations matrix
int num_jobs, num_machines;                    // Number of jobs and machines
Schedule schedule[MAX_JOBS * MAX_MACHINES];    // Array to store the schedule
pthread_mutex_t machine_mutex[MAX_MACHINES];   // Mutex for each machine
pthread_mutex_t job_mutex[MAX_JOBS];           // Mutex for each job
pthread_mutex_t schedule_mutex;                // Mutex for the schedule array
int machine_schedules[MAX_MACHINES][MAX_TIME]; // Machine time slots (0=free, 1=busy)
int machine_end_time[MAX_MACHINES] = {0};      // End time for each machine

// Function declarations
void *scheduleJobs(void *thread_args);
void initializeMutex(int num_threads);
void readJobDataFromFile(char *input_file);
void createThreadsAndAssignJobs(int num_threads, pthread_t *threads, ThreadArgs *thread_args);
void writeScheduleToOutputFile(char *output_file);
int calculateMakespan();
void destroyMutex();
int max(int a, int b);

int main(int argc, char *argv[])
{
    // Check for correct usage
    if (argc < 4)
    {
        printf("Usage: %s <input_file> <output_file> <num_threads>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    int num_threads = atoi(argv[3]);

    pthread_t threads[num_threads];      // Thread handles
    ThreadArgs thread_args[num_threads]; // Arguments for threads

    gettimeofday(&start, NULL);                                    // Start timing
    readJobDataFromFile(input_file);                               // Read job data
    initializeMutex(num_threads);                                  // Initialize mutexes
    createThreadsAndAssignJobs(num_threads, threads, thread_args); // Create threads and assign jobs
    gettimeofday(&end, NULL);                                      // End timing

    // Calculate elapsed time
    double time_taken = (end.tv_sec - start.tv_sec) * 1e6;
    time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;

    writeScheduleToOutputFile(output_file); // Write schedule to output file

    destroyMutex(); // Destroy mutexes

    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}

// Check if a machine is busy during a given time interval
bool isOverlapping(int machine, int start_time, int end_time)
{
    for (int t = start_time; t < end_time; t++)
    {
        if (machine_schedules[machine][t] == 1)
        {
            return true;
        }
    }
    return false;
}

// Thread function to schedule jobs assigned to this thread
void *scheduleJobs(void *thread_args)
{
    ThreadArgs *args = (ThreadArgs *)thread_args;
    int start_job_index = args->start_job_index;
    int end_job_index = args->end_job_index;

    for (int i = start_job_index; i < end_job_index; i++)
    {
        printf("Thread %ld: Working on Job %d\n", pthread_self(), i);
        pthread_mutex_lock(&job_mutex[i]);

        // Array to track job availability (when the job is ready for the next operation)
        int job_availability[MAX_JOBS] = {0};

        for (int j = 0; j < num_machines; j++)
        {
            int machine = jobs[i][j].machine;
            int time = jobs[i][j].time;

            pthread_mutex_lock(&machine_mutex[machine]);
            // Find the earliest available time for this machine and job
            int start_time = max(job_availability[i], machine_end_time[machine]);
            while (isOverlapping(machine, start_time, start_time + time))
            {
                start_time++;
            }

            // Update job and machine availability
            int end_time = start_time + time;
            job_availability[i] = end_time;
            machine_end_time[machine] = end_time;

            // Mark the machine as busy for this time interval
            for (int t = start_time; t < end_time; t++)
            {
                machine_schedules[machine][t] = 1;
            }

            // Update the global schedule (protected by mutex)
            pthread_mutex_lock(&schedule_mutex);
            schedule[i * num_machines + j].job = i;
            schedule[i * num_machines + j].machine = machine;
            schedule[i * num_machines + j].start_time = start_time;
            schedule[i * num_machines + j].end_time = end_time;
            pthread_mutex_unlock(&schedule_mutex);

            // Debug output
            printf("Thread %ld: Job %d - Machine %d - Start %d - End %d\n", pthread_self(), i, machine, start_time, end_time);
            printf("Thread %ld: Job Availability: %d\n", pthread_self(), job_availability[i]);
            printf("Thread %ld: Machine End Time: %d\n", pthread_self(), machine_end_time[machine]);

            pthread_mutex_unlock(&machine_mutex[machine]);
        }
        pthread_mutex_unlock(&job_mutex[i]);
    }

    return NULL;
}

// Initialize all mutexes for machines, jobs, and schedule
void initializeMutex(int num_threads)
{
    for (int i = 0; i < MAX_MACHINES; i++)
    {
        pthread_mutex_init(&machine_mutex[i], NULL);
    }
    for (int i = 0; i < MAX_JOBS; i++)
    {
        pthread_mutex_init(&job_mutex[i], NULL);
    }
    pthread_mutex_init(&schedule_mutex, NULL);
}

// Read job and machine data from input file
void readJobDataFromFile(char *input_file)
{
    char openPath[100] = "../ft/";
    strncat(openPath, input_file, strlen(input_file));
    printf("Opening file %s\n", openPath);

    FILE *file = fopen(openPath, "r");
    if (file == NULL)
    {
        printf("Could not open file\n");
        exit(1);
    }

    fscanf(file, "%d %d", &num_jobs, &num_machines);

    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            fscanf(file, "%d %d", &jobs[i][j].machine, &jobs[i][j].time);
        }
    }

    fclose(file);
}

// Create threads and assign jobs to each thread
void createThreadsAndAssignJobs(int num_threads, pthread_t *threads, ThreadArgs *thread_args)
{
    // Set all machine schedules to free (0)
    memset(machine_schedules, 0, sizeof(machine_schedules));

    int jobs_per_thread = num_jobs / num_threads;
    for (int i = 0; i < num_threads; i++)
    {
        thread_args[i].start_job_index = i * jobs_per_thread;
        thread_args[i].end_job_index = (i == num_threads - 1) ? num_jobs : (i + 1) * jobs_per_thread;
        pthread_create(&threads[i], NULL, scheduleJobs, &thread_args[i]);
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

// Write the final schedule and makespan to the output file
void writeScheduleToOutputFile(char *output_file)
{
    char resultsPath[100] = "../Resultados/";
    strncat(resultsPath, output_file, strlen(output_file));

    FILE *output = fopen(resultsPath, "w");
    if (output == NULL)
    {
        printf("Could not open or create output file\n");
        exit(1);
    }

    for (int i = 0; i < num_jobs * num_machines; i++)
    {
        fprintf(output, "Job %d - Machine %d - Start %d - End %d\n", schedule[i].job, schedule[i].machine, schedule[i].start_time, schedule[i].end_time);
    }

    // Calculate and write makespan
    int makespan = calculateMakespan();
    fprintf(output, "The makespan is %d\n", makespan);

    fclose(output);
}

// Calculate the makespan (maximum end time of all scheduled operations)
int calculateMakespan()
{
    int makespan = 0;
    for (int i = 0; i < num_jobs * num_machines; i++)
    {
        if (schedule[i].end_time > makespan)
        {
            makespan = schedule[i].end_time;
        }
    }
    return makespan;
}

// Destroy all mutexes before program exit
void destroyMutex()
{
    for (int i = 0; i < MAX_MACHINES; i++)
    {
        pthread_mutex_destroy(&machine_mutex[i]);
    }
    for (int i = 0; i < MAX_JOBS; i++)
    {
        pthread_mutex_destroy(&job_mutex[i]);
    }
    pthread_mutex_destroy(&schedule_mutex);
}

// Utility function to return the maximum of two integers
int max(int a, int b)
{
    return (a > b) ? a : b;
}