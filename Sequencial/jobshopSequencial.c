#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h> 
#include <string.h>

#define MAX_JOBS 100
#define MAX_MACHINES 100

int main() {
    
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
    if (file == NULL) {
        printf("Error: File not found\n");
        exit(1);
    }

    int num_jobs, num_machines;

    fscanf(file, "%d %d", &num_jobs, &num_machines);

    int jobs[num_jobs][num_machines*2];
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines*2; j++) {
            fscanf(file, "%d", &jobs[i][j]);
        }
    }

    fclose(file);

    int processing_time[MAX_JOBS][MAX_MACHINES] = {0};
    int completion_time[MAX_JOBS][MAX_MACHINES] = {0};
    int machine_available_time[MAX_MACHINES] = {0};
    int job_completed[MAX_JOBS] = {0};
    
    // Initialize processing_time from jobs array
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines; j++) {
            processing_time[i][j] = jobs[i][j*2+1];
        }
    }
    
    // Schedule jobs
    for (int i = 0; i < num_jobs * num_machines; i++) {
        int min_completion_time = INT_MAX;
        int next_job = -1;
        int next_machine = -1;

        // Find the next job to schedule
        for (int j = 0; j < num_jobs; j++) {
            for (int k = 0; k < num_machines; k++) {
                if (!job_completed[j] && jobs[j][k*2] == i % num_machines) {
                    int new_completion_time = (i > 0 ? completion_time[j][k-1] : 0) + processing_time[j][k];
                    if (new_completion_time < min_completion_time) {
                        min_completion_time = new_completion_time;
                        next_job = j;
                        next_machine = k;
                    }
                }
            }
        }

        // Update completion time and machine availability
        completion_time[next_job][next_machine] = min_completion_time;
        machine_available_time[next_machine] = min_completion_time;

        // Check if the job is completed
        if (next_machine == num_machines - 1) {
            job_completed[next_job] = 1;
        }
    }

    // Print the completion time for each job on each machine
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines; j++) {
            printf("Job %d on Machine %d: Completion Time = %d\n", i, j, completion_time[i][j]);
        }
    }
    
    return 0;
}
