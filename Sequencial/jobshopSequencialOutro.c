#include <stdio.h>
#include <stdlib.h>

#define NUM_JOBS 3
#define NUM_MACHINES 3

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

int max(int a, int b) {
    return (a > b) ? a : b;
}

int main() {
    Job jobs[NUM_JOBS][NUM_MACHINES];
    int machine_time[NUM_MACHINES] = {0};
    Schedule schedule[NUM_JOBS * NUM_MACHINES];
    int current_operation[NUM_JOBS] = {0};

    // Read job data from file
    FILE *file = fopen("../ft/ft03.jss", "r");
    if (file == NULL) {
        printf("Could not open file\n");
        return 1;
    }

    int num_jobs, num_machines;
    fscanf(file, "%d %d", &num_jobs, &num_machines);

    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines; j++) {
            fscanf(file, "%d %d", &jobs[i][j].machine, &jobs[i][j].time);
        }
    }

    fclose(file);

    // Initialize machine availability times
    int machine_availability[NUM_MACHINES] = {0};
    // Initialize job end times
    int job_end_times[NUM_JOBS] = {0};

    // Populate the schedule array and update machine availability times
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines; j++) {
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

    // Print the schedule
    for (int i = 0; i < num_jobs * num_machines; i++) {
        printf("Job %d starts at time %d on machine %d\n", schedule[i].job, schedule[i].start_time, schedule[i].machine);
    }
    // Initialize makespan
    int makespan = 0;

    // Calculate makespan
    for (int i = 0; i < num_jobs * num_machines; i++) {
        if (schedule[i].end_time > makespan) {
            makespan = schedule[i].end_time;
        }
    }

    printf("The makespan is %d\n", makespan);

    return 0;
}