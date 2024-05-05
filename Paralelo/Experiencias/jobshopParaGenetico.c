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
    int job_finish_time[MAX_JOBS][MAX_MACHINES];
    int makespan = 0;
    int i, j;

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
    for (i = 0; i < num_jobs; i++)
    {
        for (j = 0; j < num_machines; j++)
        {
            fscanf(file, "%d %d", &machine_assignment[i][j], &processing_times[i][j]);
        }
    }

    fclose(file);

    // Initialize finish times
    for (i = 0; i < num_machines; i++)
    {
        machine_finish_time[i] = 0;
    }

// Main scheduling loop
#pragma omp parallel for private(i, j)
    for (i = 0; i < num_jobs; i++)
    {
        int job_finish = 0;
        for (j = 0; j < num_machines; j++)
        {
            int machine = machine_assignment[i][j];
            int start_time = machine_finish_time[machine] > job_finish ? machine_finish_time[machine] : job_finish;
            job_finish = start_time + processing_times[i][j];
            machine_finish_time[machine] = job_finish;
            job_finish_time[i][j] = job_finish;
        }
    }

    // Find maximum finish time (makespan)
    for (i = 0; i < num_machines; i++)
    {
        if (machine_finish_time[i] > makespan)
        {
            makespan = machine_finish_time[i];
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
            printf(" M%d-%d", machine, job_finish_time[i][j]);
        }
        printf("\n");
    }

    // Output makespan
    printf("\nTotal time for job completion: %d units of time\n", makespan);

    return 0;
}
