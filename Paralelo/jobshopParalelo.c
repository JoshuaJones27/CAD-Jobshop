#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_JOBS 3
#define NUM_MACHINES 3

typedef struct
{
    int machine;
    int time;
} Job;

typedef struct
{
    int job;
    int machine;
    int start_time;
    int end_time;
} Schedule;

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int main()
{
    Job jobs[NUM_JOBS][NUM_MACHINES];
    int machine_time[NUM_MACHINES] = {0};
    Schedule schedule[NUM_JOBS * NUM_MACHINES];
    int current_operation[NUM_JOBS] = {0};

    // Declare and initialize machine_availability and job_end_times
    int machine_availability[NUM_MACHINES];
    int job_end_times[NUM_JOBS];
    for (int i = 0; i < NUM_MACHINES; i++)
    {
        machine_availability[i] = 0;
    }
    for (int i = 0; i < NUM_JOBS; i++)
    {
        job_end_times[i] = 0;
    }

    // Read job data from file
    FILE *file = fopen("../ft/ft03.jss", "r");
    if (file == NULL)
    {
        printf("Could not open file\n");
        return 1;
    }

    int num_jobs, num_machines;
    fscanf(file, "%d %d", &num_jobs, &num_machines);

// Parallelize reading job data from file
#pragma omp parallel for
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            fscanf(file, "%d %d", &jobs[i][j].machine, &jobs[i][j].time);
        }
    }

    fclose(file);

    // Create an array of job indices
    int job_order[num_jobs];
    for (int i = 0; i < num_jobs; i++)
    {
        job_order[i] = i;
    }

    // Sort the job indices based on the total processing time of each job
    for (int i = 0; i < num_jobs - 1; i++)
    {
        for (int j = 0; j < num_jobs - i - 1; j++)
        {
            int total_time_j = 0;
            int total_time_j_plus_1 = 0;
            for (int k = 0; k < num_machines; k++)
            {
                total_time_j += jobs[job_order[j]][k].time;
                total_time_j_plus_1 += jobs[job_order[j + 1]][k].time;
            }
            if (total_time_j > total_time_j_plus_1)
            {
                // Swap job_order[j] and job_order[j + 1]
                int temp = job_order[j];
                job_order[j] = job_order[j + 1];
                job_order[j + 1] = temp;
            }
        }
    }

    // Schedule the jobs in the sorted order
    for (int i = 0; i < num_jobs; i++)
    {
        int job = job_order[i];
        for (int j = 0; j < num_machines; j++)
        {
            // Get the machine for the job
            int machine = jobs[job][j].machine;

            // Schedule the job at the time the machine and the job both become available
            int start_time = max(machine_availability[machine], job_end_times[job]);

            // Update the schedule
            schedule[job * num_machines + j].start_time = start_time;
            schedule[job * num_machines + j].end_time = start_time + jobs[job][j].time;

            // Update machine availability time and job end time
            machine_availability[machine] = schedule[job * num_machines + j].end_time;
            job_end_times[job] = schedule[job * num_machines + j].end_time;
        }
    }

    // Populate the schedule array and update machine availability times
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            // Get the machine for the job
            int machine = jobs[i][j].machine; // Adjust for 0-indexing

            // Schedule the job at the time the machine and the job both become available
            int start_time = max(machine_availability[machine], job_end_times[i]);

            // Update the schedule
            schedule[i * num_machines + j].job = i;
            schedule[i * num_machines + j].machine = machine;
            schedule[i * num_machines + j].start_time = start_time;
            schedule[i * num_machines + j].end_time = start_time + jobs[i][j].time;

            // Update machine availability time and job end time
            machine_availability[machine] = schedule[i * num_machines + j].end_time;
            job_end_times[i] = schedule[i * num_machines + j].end_time;
        }
    }

// Parallelize printing the schedule
#pragma omp parallel for
    for (int i = 0; i < num_jobs * num_machines; i++)
    {
        printf("Job %d starts at time %d on machine %d\n", schedule[i].job, schedule[i].start_time, schedule[i].machine);
    }
    // Initialize makespan
    int makespan = 0;

// Parallelize calculation of makespan
#pragma omp parallel for reduction(max : makespan)
    for (int i = 0; i < num_jobs * num_machines; i++)
    {
        makespan = max(makespan, schedule[i].end_time);
    }

    printf("The makespan is %d\n", makespan);

    return 0;
}