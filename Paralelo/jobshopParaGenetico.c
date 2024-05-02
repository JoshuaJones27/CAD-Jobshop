#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_MACHINES 100
#define MAX_JOBS 100

int main()
{
    FILE *file;
    int num_jobs, num_machines;
    int processing_times[MAX_JOBS][MAX_MACHINES];
    int machine_assignment[MAX_JOBS][MAX_MACHINES];
    int machine_finish_time[MAX_MACHINES];
    int job_finish_time[MAX_JOBS];
    int job_queue[MAX_JOBS];
    int i, j, k;

    // Open the file
    file = fopen("../ft/ft03.jss", "r");
    if (file == NULL)
    {
        printf("Error opening file ft03.jss\n");
        exit(1);
    }

    // Read number of jobs and machines
    fscanf(file, "%d %d", &num_jobs, &num_machines);

    // Read processing times and machine assignments
    // Read processing times and machine assignments
    for (i = 0; i < num_jobs; i++)
    {
        for (j = 0; j < num_machines; j++)
        {
            fscanf(file, "%d %d", &machine_assignment[i][j], &processing_times[i][j]);
        }
    }

    fclose(file);

    // Initialize finish times
    for (i = 0; i < num_jobs; i++)
    {
        job_finish_time[i] = 0;
    }
    for (i = 0; i < num_machines; i++)
    {
        machine_finish_time[i] = 0;
    }

// Initialize job queue
#pragma omp parallel for
    for (i = 0; i < num_jobs; i++)
    {
        job_queue[i] = i;
    }

// Main scheduling loop
#pragma omp parallel for private(i, j, k)
    for (i = 0; i < num_jobs; i++)
    {
        int job = job_queue[i];
        for (j = 0; j < num_machines; j++)
        {
            int machine = machine_assignment[job][j];
            // Find the finish time of the job on the previous machine in the order
            int prev_finish_time = 0;
            if (j > 0)
            {
                int prev_machine = machine_assignment[job][j - 1];
                prev_finish_time = machine_finish_time[prev_machine];
            }
            // Calculate the start time of the job on the current machine
            int start_time = prev_finish_time > job_finish_time[job] ? prev_finish_time : job_finish_time[job];
            start_time = machine_finish_time[machine] > start_time ? machine_finish_time[machine] : start_time;
            // Update finish times
            machine_finish_time[machine] = start_time + processing_times[job][j];
            job_finish_time[job] = machine_finish_time[machine];
        }
    }

    // Output job schedule
    printf("Job schedule:\n");
    for (i = 0; i < num_jobs; i++)
    {
        printf("Job %d:", i);
        for (j = 0; j < num_machines; j++)
        {
            int machine = machine_assignment[i][j];
            printf(" M%d-%d", machine, job_finish_time[i]);
        }
        printf("\n");
    }

    // Find maximum finish time (makespan)
    int makespan = 0;
    for (i = 0; i < num_machines; i++)
    {
        if (machine_finish_time[i] > makespan)
        {
            makespan = machine_finish_time[i];
        }
    }

    // Output makespan
    printf("\nTotal time for job completion: %d units of time\n", makespan);

    // Output total execution time
    printf("Total execution time: %f seconds\n", omp_get_wtime());

    return 0;
}
