#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#define MAX_JOBS 100
#define MAX_MACHINES 100

void write_performance(int num_jobs, int num_machines, int completion_time[MAX_JOBS][MAX_MACHINES]);

int main()
{

    clock_t start_time, end_time;
    double total_time;

    // Código para abrir só o ficheiro ft06.jss
    // FILE *file = fopen("../ft/ft06.jss", "r");
    // if (file == NULL) {
    //     printf("Error: File not found\n");
    //     exit(1);
    // }

    char filename[50];
    printf("Enter the name of the file you want to open (e.g., ft06.jss): ");
    scanf("%s", filename);

    char filepath[60] = "../ft/";
    strcat(filepath, filename);

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        exit(1);
    }

    int num_jobs, num_machines;

    fscanf(file, "%d %d", &num_jobs, &num_machines);

    int jobs[num_jobs][num_machines * 2];
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines * 2; j++)
        {
            fscanf(file, "%d", &jobs[i][j]);
        }
    }

    fclose(file);

    start_time = clock();

    int processing_time[MAX_JOBS][MAX_MACHINES] = {0};
    int completion_time[MAX_JOBS][MAX_MACHINES] = {0};
    int machine_available_time[MAX_MACHINES] = {0};
    int job_completed[MAX_JOBS] = {0};

    // Initialize processing_time from jobs array
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            processing_time[i][j] = jobs[i][j * 2 + 1];
        }
    }

    // Schedule jobs
    for (int current_task = 0; current_task < num_jobs * num_machines; current_task++)
    {
        int min_completion_time = INT_MAX;
        int next_job = -1;
        int next_machine = -1;

        // Find the next job to schedule
        for (int j = 0; j < num_jobs; j++)
        {
            if (!job_completed[j])
            {
                for (int k = 0; k < num_machines; k++)
                {
                    if (jobs[j][k * 2] == current_task % num_machines)
                    {
                        int start_time = (k > 0 ? completion_time[j][k - 1] : 0);
                        int new_completion_time = start_time + processing_time[j][k];

                        if (new_completion_time < min_completion_time)
                        {
                            min_completion_time = new_completion_time;
                            next_job = j;
                            next_machine = k;
                        }
                    }
                }
            }
        }

        // Update completion time and machine availability
        if (next_job != -1 && next_machine != -1)
        {
            completion_time[next_job][next_machine] = min_completion_time;
            machine_available_time[next_machine] = min_completion_time;
            job_completed[next_job] = 1; // Mark job as completed
        }
    }

    // Output scheduling details for each machine
    for (int m = 0; m < num_machines; m++)
    {
        printf("Machine %d:\n", m);
        for (int j = 0; j < num_jobs; j++)
        {
            for (int k = 0; k < num_machines; k++)
            {
                if (jobs[j][k * 2] == m && completion_time[j][k] > 0)
                {
                    int start_time = (k > 0 ? completion_time[j][k - 1] : 0);
                    int end_time = completion_time[j][k];
                    printf("Job %d, Task %d: Start Time = %d, End Time = %d\n", j, k, start_time, end_time);
                    break; // Move to the next machine
                }
            }
        }
        printf("\n");
    }

    // Printar na consola
    // for (int i = 0; i < num_jobs; i++) {
    //     for (int j = 0; j < num_machines; j++) {
    //         printf("Job %d on Machine %d: Completion Time = %d\n", i, j, completion_time[i][j]);
    //     }
    // }

    // Calculate and output optimal schedule length
    int optimal_schedule_length = completion_time[num_jobs - 1][num_machines - 1];
    printf("Optimal Schedule Length: %d\n", optimal_schedule_length);

    // Output scheduling details for each machine
    for (int m = 0; m < num_machines; m++)
    {
        printf("Machine %d:\n", m);
        for (int j = 0; j < num_jobs; j++)
        {
            for (int k = 0; k < num_machines; k++)
            {
                if (jobs[j][k * 2] == m && !job_completed[j])
                {
                    int start_time = (k > 0 ? completion_time[j][k - 1] : 0);
                    int end_time = start_time + processing_time[j][k];
                    printf("Job %d, Task %d: Start Time = %d, End Time = %d\n", j, k, start_time, end_time);
                    completion_time[j][k] = end_time;
                    machine_available_time[m] = end_time;
                    job_completed[j] = 1;
                    break; // Move to the next machine
                }
            }
        }
        printf("\n");
    }

    // Open the output file
    char output_filename[50];
    printf("Enter the name of the output file: ");
    scanf("%s", output_filename);
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        printf("Error: Failed to open output file\n");
        exit(1);
    }

    // Write the completion times to the output file
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            fprintf(output_file, "Job %d on Machine %d: Completion Time = %d\n", i, j, completion_time[i][j]);
        }
    }

    end_time = clock();
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    fprintf(output_file, "Execution time: %f seconds\n", total_time);

    write_performance(num_jobs, num_machines, completion_time);

    // Close the output file
    fclose(output_file);

    printf("Execution time: %f seconds\n", total_time);

    return 0;
}

void write_performance(int num_jobs, int num_machines, int completion_time[MAX_JOBS][MAX_MACHINES])
{
    int last_task_end_time[MAX_MACHINES] = {0};
    printf("Optimal Schedule Length: %d\n", completion_time[num_jobs - 1][num_machines - 1]);
    for (int i = 0; i < num_machines; i++)
    {
        printf("Machine %d: ", i);
        for (int j = 0; j < num_jobs; j++)
        {
            if (completion_time[j][i] > 0)
            {
                int start_time = last_task_end_time[i];
                int end_time = completion_time[j][i];
                printf("Job %d, Task %d: Start Time = %d, End Time = %d\n", j, i, start_time, end_time);
                last_task_end_time[i] = end_time;
            }
        }
        printf("\n");
    }
}