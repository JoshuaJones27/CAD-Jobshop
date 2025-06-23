#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define the maximum number of jobs and machines
#define NUM_JOBS 100
#define NUM_MACHINES 100

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

// Utility function to return the maximum of two integers
int max(int a, int b)
{
    return (a > b) ? a : b;
}

int main()
{
    // Declare variables
    Job jobs[NUM_JOBS][NUM_MACHINES];           // Array to store job operations
    int machine_time[NUM_MACHINES] = {0};       // Not used in the code, can be removed
    Schedule schedule[NUM_JOBS * NUM_MACHINES]; // Array to store the schedule
    int current_operation[NUM_JOBS] = {0};      // Not used in the code, can be removed
    char file_name[50];                         // Input file name
    char output_file_name[50];                  // Output file name

    // Prompt user for the input file name
    printf("Type the file name to read\n");
    scanf("%s", file_name);

    // Construct the file path to open
    char openPath[100] = "../ft/";
    strncat(openPath, file_name, strlen(file_name));

    // Open the input file for reading
    FILE *file = fopen(openPath, "r");
    if (file == NULL)
    {
        printf("Could not open file\n");
        return 1;
    }

    int num_jobs, num_machines;
    // Read the number of jobs and machines from the file
    fscanf(file, "%d %d", &num_jobs, &num_machines);

    // Read the job data (machine and time for each operation) from the file
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            fscanf(file, "%d %d", &jobs[i][j].machine, &jobs[i][j].time);
        }
    }

    // Close the input file
    fclose(file);

    // Start the timer to measure execution time
    clock_t start = clock();

    // Arrays to track machine and job availability times
    int machine_availability[NUM_MACHINES] = {0}; // When each machine becomes available
    int job_end_times[NUM_JOBS] = {0};            // When each job becomes available

    // Populate the schedule array and update machine/job availability times
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < num_machines; j++)
        {
            // Get the machine for the current operation
            int machine = jobs[i][j].machine; // Adjust for 0-indexing if needed

            // Determine the earliest start time for this operation
            int start_time = max(machine_availability[machine], job_end_times[i]);

            // Update the schedule with job, machine, start, and end times
            schedule[i * num_machines + j].job = i;
            schedule[i * num_machines + j].machine = machine;
            schedule[i * num_machines + j].start_time = start_time;
            schedule[i * num_machines + j].end_time = start_time + jobs[i][j].time;

            // Update machine and job availability times
            machine_availability[machine] = schedule[i * num_machines + j].end_time;
            job_end_times[i] = schedule[i * num_machines + j].end_time;
        }
    }

    // Initialize makespan (total completion time)
    int makespan = 0;

    // Calculate the makespan by finding the maximum end time in the schedule
    for (int i = 0; i < num_jobs * num_machines; i++)
    {
        if (schedule[i].end_time > makespan)
        {
            makespan = schedule[i].end_time;
        }
    }

    // Stop the timer and calculate elapsed time
    clock_t end = clock();
    double time_taken = ((double)end - start) / CLOCKS_PER_SEC;

    // Prompt user for the output file name
    printf("Enter the name of the file to write the results to: ");
    scanf("%s", output_file_name);

    // Construct the output file path
    char resultsPath[100] = "../Resultados/";
    strncat(resultsPath, output_file_name, strlen(output_file_name));

    // Open the output file for writing
    FILE *output_file = fopen(resultsPath, "w");
    if (output_file == NULL)
    {
        printf("Could not open or create file %s\n", output_file_name);
        return 1;
    }

    // Print and write the schedule to the output file
    for (int i = 0; i < num_jobs * num_machines; i++)
    {
        printf("Job %d - Machine %d - Start %d - End %d\n", schedule[i].job, schedule[i].machine, schedule[i].start_time, schedule[i].end_time);
        fprintf(output_file, "Job %d - Machine %d - Start %d - End %d\n", schedule[i].job, schedule[i].machine, schedule[i].start_time, schedule[i].end_time);
    }

    // Print and write the makespan and execution time
    printf("The makespan is %d\n", makespan);
    printf("Time taken: %f seconds\n", time_taken);
    fprintf(output_file, "The makespan is %d\n", makespan);
    fprintf(output_file, "Time taken: %f seconds\n", time_taken);

    // Close the output file
    fclose(output_file);

    return 0;
}