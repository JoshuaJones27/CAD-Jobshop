#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#define MAX 200
#define MAXTIME 100000

int maxJobs, maxMac = 0;

// Matrices for job time and machine assignments
int jobTime[MAX][MAX];
int jobM[MAX][MAX];
int jobSolution[MAX][MAX];

// Matrix to track machine occupation
int Machines[MAXTIME][MAX];

void ReadFile(char *fileName);
void WriteFile(char *filename);
int JobShop();
int VerifySolution();
void ShiftingBottleneckHeuristic();
int CalculateMakespan();
int max(int a, int b);

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int main()
{

    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            jobSolution[i][j] = -1;
        }
    }

    for (int i = 0; i < MAXTIME; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            Machines[i][j] = -1;
        }
    }

    JobShop();
    int makespan = CalculateMakespan();
    printf("Makespan: %d\n", makespan);

    clock_t start_time, end_time;
    double total_time;

    // Código para abrir só o ficheiro ft03.jss
    FILE *file = fopen("../ft/ft03.jss", "r");
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

    ReadFile("../ft/ft03.jss");

    start_time = clock();

    int time = JobShop();

    end_time = clock();
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Printing the results
    printf("Job schedule:\n");
    for (int i = 0; i < maxJobs; i++)
    {
        printf("Job %d: ", i);
        for (int j = 0; j < maxMac; j++)
        {
            if (jobSolution[i][j] == -1)
            {
                printf("\t");
            }
            else
            {
                printf(" M%d-%d\t", jobM[i][j], jobSolution[i][j]);
            }
        }
        printf("\n");
    }

    printf("\nTotal time for job completion: %d units of time\n", time);
    printf("Total execution time: %.6f seconds\n", total_time);

    return 0;
}

bool VerifyPreviousOp(int job, int op)
{
    if (op > 0 && jobSolution[job][op - 1] == -1)
        return false;
    return true;
}

int InsertJob(int machine, int time, int jobDuration, int jobNr)
{
    // Check if the machine is available for the entire duration of the job
    for (int i = time; i < time + jobDuration; i++)
    {
        if (Machines[i][machine] != -1)
        {
            printf("InsertJob: Machine %d is not available at time %d\n", machine, i);
            return 0;
        }
    }

    // If the machine is available, schedule the job
    for (int i = time; i < time + jobDuration; i++)
    {
        Machines[i][machine] = jobNr;
    }
    return 1;
}

void PrintJobs(int y, int x, int totalTime)
{
    printf("\t");
    for (int i = 0; i < x; i++)
    {
        printf(" O-%d\t", i);
    }
    printf("\n");

    for (int i = 0; i < y; i++)
    {
        printf("Job %d: ", i);
        for (int j = 0; j < x; j++)
        {
            if (jobSolution[i][j] == -1)
            {
                printf("\t");
            }
            else
            {
                printf(" M%d-%d\t", jobM[i][j], jobSolution[i][j]);
            }
        }
        printf("\n");
    }
    printf("Total time: %d \n", totalTime);
}

void ReadFile(char *fileName)
{
    FILE *fpInput = fopen(fileName, "r");
    if (!fpInput)
    {
        perror("fopen");
        return;
    }

    fscanf(fpInput, "%d %d", &maxJobs, &maxMac);
    printf("ReadFile: maxJobs = %d, maxMac = %d\n", maxJobs, maxMac); // Print maxJobs and maxMac

    for (int i = 0; i < maxJobs; i++)
    {
        for (int j = 0; j < maxMac * 2; j += 2) // Increment by 2 to read pairs of numbers
        {
            fscanf(fpInput, "%d %d", &jobM[i][j / 2], &jobTime[i][j / 2]);
            printf("ReadFile: Job %d, Machine %d, Time %d\n", i, jobM[i][j / 2], jobTime[i][j / 2]); // Print jobM and jobTime values
        }
    }

    fclose(fpInput);
}

void WriteFile(char *filename)
{
    FILE *fpInput = fopen(filename, "w+");
    if (!fpInput)
    {
        perror("fopen");
        return;
    }

    fprintf(fpInput, "%d  %d\n", maxJobs, maxMac);

    for (int y = 0; y < maxJobs; y++)
    {
        for (int x = 0; x < maxMac; x++)
        {
            fprintf(fpInput, "%d  ", jobSolution[y][x]);
        }
        fprintf(fpInput, "\n");
    }

    fclose(fpInput);
}

int JobShop()
{
    int time = 0;
    int jobCompletion[MAX] = {0}; // Array to keep track of the completion time of the last operation for each job
    int machineTime[MAX] = {0};   // Array to keep track of the completion time of each machine

    // Schedule jobs in order of their total processing time
    for (int i = 0; i < maxJobs; i++)
    {
        int minTime = INT_MAX;
        int minMachine = -1;

        // Find machine with the earliest available time
        for (int j = 0; j < maxMac; j++)
        {
            if (machineTime[jobM[i][j]] < minTime)
            {
                minTime = machineTime[jobM[i][j]];
                minMachine = jobM[i][j];
            }
        }

        // Ensure total time for job completion is under 20 units
        if (minTime > 20)
        {
            printf("Total time for job completion exceeds 20 units.\n");
            return -1;
        }

        // Update machine occupation and job solution
        InsertJob(minMachine, minTime, jobTime[i][0], i);
        jobSolution[i][0] = minTime;
        jobCompletion[i] = minTime + jobTime[i][0];
        machineTime[minMachine] = jobCompletion[i]; // Update machine completion time

        // Update machine completion time for subsequent operations of the same job
        for (int j = 1; j < maxMac; j++)
        {
            InsertJob(jobM[i][j], jobCompletion[i], jobTime[i][j], i);
            jobSolution[i][j] = jobCompletion[i];
            jobCompletion[i] += jobTime[i][j];
            machineTime[jobM[i][j]] = jobCompletion[i]; // Update machine completion time
        }
    }

    // Calculate total time for job completion
    int totalJobCompletionTime = 0;
    for (int i = 0; i < maxJobs; i++)
    {
        if (jobCompletion[i] > totalJobCompletionTime)
        {
            totalJobCompletionTime = jobCompletion[i];
        }
    }

    return totalJobCompletionTime;
}

bool verifySolution()
{
    // Check if each job is scheduled on all its machines in the correct order
    for (int job = 0; job < maxJobs; job++)
    {
        for (int machine = 0; machine < maxMac - 1; machine++)
        {
            if (jobSolution[job][machine] >= jobSolution[job][machine + 1])
            {
                return false;
            }
        }
    }

    // Check if no two jobs are scheduled on the same machine at the same time
    for (int time = 0; time < MAXTIME; time++)
    {
        for (int machine = 0; machine < MAX; machine++)
        {
            int job = Machines[time][machine];
            if (job != -1)
            {
                if (time < jobSolution[job][0] || time > jobSolution[job][maxMac - 1])
                {
                    return false;
                }
            }
        }
    }

    // Check if the total time for job completion is equal to the maximum completion time among all jobs
    int maxCompletionTime = 0;
    for (int job = 0; job < maxJobs; job++)
    {
        if (jobSolution[job][maxMac - 1] > maxCompletionTime)
        {
            maxCompletionTime = jobSolution[job][maxMac - 1];
        }
    }
    if (maxCompletionTime != JobShop())
    {
        return false;
    }

    return true;
}

void ShiftingBottleneckHeuristic()
{
    int schedule[MAX][MAX];  // The schedule for each machine
    int completionTime[MAX]; // The completion time for each job

    // Initialize the schedule and completion time
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            schedule[i][j] = -1;
        }
        completionTime[i] = 0;
    }

    // Repeat until all jobs are scheduled
    while (true)
    {
        int maxTime = -1;
        int bottleneck = -1;

        // Find the bottleneck machine
        for (int i = 0; i < maxMac; i++)
        {
            int time = 0;
            for (int j = 0; j < maxJobs; j++)
            {
                if (schedule[i][j] == -1)
                {
                    time += jobTime[j][i];
                }
            }
            if (time > maxTime)
            {
                maxTime = time;
                bottleneck = i;
            }
        }

        // If no bottleneck machine is found, all jobs are scheduled
        if (bottleneck == -1)
        {
            break;
        }

        // Schedule the jobs on the bottleneck machine
        for (int i = 0; i < maxJobs; i++)
        {
            if (schedule[bottleneck][i] == -1)
            {
                schedule[bottleneck][i] = max(completionTime[i], maxTime);
                completionTime[i] = schedule[bottleneck][i] + jobTime[i][bottleneck];
                maxTime = completionTime[i];
            }
        }
    }

    // Print the schedule
    for (int i = 0; i < maxMac; i++)
    {
        printf("Machine %d: ", i);
        for (int j = 0; j < maxJobs; j++)
        {
            printf("%d ", schedule[i][j]);
        }
        printf("\n");
    }
}

int CalculateMakespan()
{
    int makespan = 0;
    for (int i = 0; i < maxJobs; i++)
    {
        for (int j = 0; j < maxMac; j++)
        {
            if (jobSolution[i][j] > makespan)
            {
                makespan = jobSolution[i][j];
            }
        }
    }
    return makespan;
}