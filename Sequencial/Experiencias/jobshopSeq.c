#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

#define MAX_JOBS 100
#define MAX_MACHINES 100

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX 1000
#define MAXTIME 1000000

int maxJobs, maxMachines = 0;

int thread_count;

int jobTime[MAX][MAX];
int jobMachine[MAX][MAX];

int jobSolution[MAX_JOBS][MAX_MACHINES];
int Machines[MAXTIME][MAX];

void ReadFile(char *filename)
{
    // Open the FT03 file
    FILE *file = fopen("../ft/ft03.jss", "r");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        exit(1);
    }

    char buffer[2048 * 10];

    int x, y = 0;

    // Read the number of jobs and machines from the file
    fscanf(file, "%d %d", &maxJobs, &maxMachines);
    printf("Matrix size:\nNr Lines: %d\nNr Columns: %d\n", maxJobs, maxMachines);

    // Read the file content into the buffer
    fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), file);

    char *delimitar = "\r\n";

    char *pointer = strtok(buffer, delimitar);

    int nr, job, time, machine = 0;
    long i = 0;

    // Parse the buffer and populate the jobMachine and jobTime arrays
    while (pointer != NULL)
    {
        nr = atoi(pointer);

        if (i == maxMachines * 2)
        {
            machine = 0;
            time = 0;
            i = 0;
            job++;
        }

        if (i % 2 == 0)
        {
            jobMachine[job][machine] = nr;
            machine++;
        }
        else
        {
            jobTime[job][time] = nr;
            time++;
        }

        pointer = strtok(NULL, delimitar);
        i++;
    }

    fclose(file);
}

void WriteFile(char *filename)
{
    FILE *file = fopen(filename, "w+");
    if (file == NULL)
    {
        printf("Error: Unable to open file\n");
        exit(1);
    }
    for (int i = 0; i < maxJobs; i++)
    {
        for (int j = 0; j < maxMachines; j++)
        {
            // Rever este codigo Machines por jobSolution
            fprintf(file, "%d ", Machines[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Insert Jobs
int InsertJob(int machine, int time, int jobDuration, int jobNr)
{
    int slots = 0;
    for (int i = time; i < time; i++)
    {
        if (Machines[i][machine] == -1)
        {
            slots++;
        }
    }

    if (slots == jobDuration)
    {
        for (int i = time; i < time + jobDuration; i++)
        {
            Machines[i][machine] = jobNr;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

// Printjobs
void ListJobs(int y, int x, int totalTime)
{
    print("\t");
    for (int i = 0; i < x; i++)
    {
        printf(" 0-%d ", i);
    }
    printf("\n");

    for (int i = 0; i < y; i++)
    {
        printf("Job %d\t", i);
        for (int j = 0; j < x; j++)
        {
            // Rever este codigo jobMachine e jobTime por jobSolution
            printf(" %d-%d ", jobMachine[i][j], jobTime[i][j]);
        }
        printf("\n");
    }
    print("Total time: %d\n", totalTime);
}

bool verifyPreviousOp(int job, int op)
{
    if (op > 0)
    {
        if (jobSolution[job][op - 1] == -1)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
}

int countMachineSlots(int startTime, int endTime, int jobNr, int machineNr)
{
    int count = 0;
    for (int i = startTime; i < endTime; i++)
    {
        if (Machines[i][machineNr] == jobNr)
        {
            count++;
        }
    }
    return count;
}

int VerifySolution()
{
    for (int i = 0; i < maxJobs; i++)
        for (int j = 0; j < maxMachines; j++)
        {

            if (j > 1 && jobSolution[i][j - 1] > jobSolution[i][j])
            {
                return 1;
            }
            int r = countMachineSlots(jobSolution[i][j], jobSolution[i][j] + jobTime[i][j], i, jobMachine[i][j]);

            if (r == 0)
                return 2;
            if (r != jobTime[i][j])
                return 3;
        }

    return 0;
}

void printMachineArray(int time)
{
    print("\t");
    for (int i = 0; i < maxMachines; i++)
    {
        printf("Machine %d: ", i);
        for (int j = 0; j < time; j++)
        {
            printf("%d ", Machines[j][i]);
        }
        printf("\n");
    }

    printf("\n");
    for (int i = 0; i < time; i++)
    {
        printf("Time %d: ", i);
        for (int j = 0; j < maxMachines; j++)
        {
            printf("%d ", Machines[i][j]);
        }
        printf("\n");
    }
}

int countMachineSlots(int startTime, int endTime, int jobNr, int machineNr)
{
    int count = 0;
    for (int i = startTime; i < endTime; i++)
    {
        if (Machines[i][machineNr] == jobNr)
        {
            count++;
        }
    }
    return count;
}

int main()
{

    int num_jobs, num_machines;

    read_input("../ft/ft03.jss", &num_jobs, &num_machines);

    write_output(num_jobs, num_machines);

    return 0;
}

void read_input(const char *filename, int *num_jobs, int *num_machines, Job jobs[MAX_JOBS])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        exit(1);
    }

    fscanf(file, "%d %d", num_jobs, num_machines);

    for (int i = 0; i < *num_jobs; i++)
    {
        int num_operations;
        fscanf(file, "%d", &num_operations);
        jobs[i].num_operations = num_operations;

        for (int j = 0; j < num_operations; j++)
        {
            fscanf(file, "%d %d", &jobs[i].operations[j].machine, &jobs[i].operations[j].duration);
        }
    }

    fclose(file);
}

void schedule_jobs(int num_jobs, Job jobs[MAX_JOBS], int completion_time[MAX_JOBS][MAX_MACHINES])
{
    int machine_available_time[MAX_MACHINES] = {0};

    for (int j = 0; j < num_jobs; j++)
    {
        // Sort operations based on machine index
        qsort(jobs[j].operations, jobs[j].num_operations, sizeof(Operation), compare);

        int start_time = 0;
        for (int op = 0; op < jobs[j].num_operations; op++)
        {
            int machine = jobs[j].operations[op].machine;
            int duration = jobs[j].operations[op].duration;

            int operation_start_time = (machine_available_time[machine] > start_time) ? machine_available_time[machine] : start_time;

            completion_time[j][machine] = operation_start_time + duration;

            machine_available_time[machine] = completion_time[j][machine];

            start_time = completion_time[j][machine];
        }
    }
}

void write_output(int num_jobs, int num_machines, int completion_time[MAX_JOBS][MAX_MACHINES])
{
    printf("Job Schedule:\n");
    for (int i = 0; i < num_jobs; i++)
    {
        printf("Job %d:\t", i);
        for (int j = 0; j < num_machines; j++)
        {
            printf("M%d: %d\t", j, completion_time[i][j]);
        }
        printf("\n");
    }
}
