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
int max(int a, int b);

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void initialize_arrays()
{
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            jobSolution[i][j] = UNASSIGNED;
        }
    }

    for (int i = 0; i < MAXTIME; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            Machines[i][j] = UNASSIGNED;
        }
    }
}

int main()
{

    initialize_arrays();

    clock_t start_time, end_time;
    double total_time;

    read_file("../ft/ft03.jss");

    start_time = clock();

    int time = job_shop();

    end_time = clock();
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Printing the results
    printf("Job schedule:\n");
    for (int i = 0; i < maxJobs; i++)
    {
        printf("Job %d: ", i);
        for (int j = 0; j < maxMac; j++)
        {
            if (jobSolution[i][j] == UNASSIGNED)
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
    if (op > 0 && jobSolution[job][op - 1] == UNASSIGNED)
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

    for (int i = 0; i < maxJobs; i++)
    {
        for (int j = 0; j < maxMac * 2; j += 2) // Increment by 2 to read pairs of numbers
        {
            fscanf(fpInput, "%d %d", &jobM[i][j / 2], &jobTime[i][j / 2]);
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
    int machineTime[MAX] = {0}; // Array to keep track of the completion time of each machine

    // Schedule jobs in order of their total processing time
    for (int i = 0; i < maxJobs; i++)
    {
        int jobCompletionTime = 0;

        // Schedule operations of the current job sequentially
        for (int j = 0; j < maxMac; j++)
        {
            int machine = jobM[i][j];
            int duration = jobTime[i][j];

            // Find the earliest available time on the current machine
            int earliestAvailableTime = machineTime[machine];

            // Check if there are dependencies with previous operations
            if (j > 0)
            {
                int prevOperationEndTime = jobSolution[i][j - 1] + jobTime[i][j - 1];
                earliestAvailableTime = max(earliestAvailableTime, prevOperationEndTime);
            }

            // Update the machine occupation and job solution
            for (int t = earliestAvailableTime; t < earliestAvailableTime + duration; t++)
            {
                Machines[t][machine] = i;
            }

            // Update completion time for the current job
            jobSolution[i][j] = earliestAvailableTime;
            jobCompletionTime = earliestAvailableTime + duration;

            // Update machine completion time
            machineTime[machine] = jobCompletionTime;
        }
    }

    // Find the maximum completion time among all jobs
    int maxCompletionTime = 0;
    for (int i = 0; i < maxJobs; i++)
    {
        int jobEndTime = jobSolution[i][maxMac - 1] + jobTime[i][maxMac - 1];
        if (jobEndTime > maxCompletionTime)
        {
            maxCompletionTime = jobEndTime;
        }
    }

    return maxCompletionTime;
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
