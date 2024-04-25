#include "headers.h"
#include <math.h>

int ProStart = 0;
int finished_flag = 0;
int lastProcess_flag = 0;
char WriteBuff[100];

float averageWTA = 0;
float averageWt = 0;
float CPU_UTILIZATION = 0;

typedef struct
{
    long mtype;
    int ID;
    int ArrTime;
    int RunTime;
    int Priority;
    int memsize;

} msgbuff;

void handle(int signum)
{
    int x;
    wait(&x);
    finished_flag = 1;
}

void wirte_perf_file(float cpu_ut, float ta, float avg_wait, float std_wta)
{
    FILE *sch_perf_file = fopen("scheduler.perf", "w");
    fprintf(sch_perf_file, "CPU utilization = %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f", cpu_ut, ta, avg_wait, std_wta);
    fclose(sch_perf_file);
}

int main(int argc, char **argv)
{
    FILE *fPointer;
    // FILE *fPointer2;
    fPointer = fopen("Scheduler.log", "w");
    // fPointer2 = fopen("scheduler.perf", "w");
    fprintf(fPointer, "#At\ttime\tx\tprocess y\tstate\t\tarr\tw\ttotal\tz\tremain\ty\twait\tk\n");

    int algorithm = atoi(argv[1]);
    process p, running_process;
    running_process.ID = 0;
    running_process.Priority = -1;
    process *puff;
    heap_t *process_list = (heap_t *)calloc(1, sizeof(heap_t));
    heap_t *ProFinished = (heap_t *)calloc(1, sizeof(heap_t));
    msgbuff data;
    int msgqid1 = msgget((key_t)123, 0644 | IPC_CREAT);
    initClk();
    int rec_val = msgrcv(msgqid1, &data, sizeof(data) - sizeof(data.mtype), 0, !IPC_NOWAIT);
    while (true)
    {
        signal(SIGINT, handle);
        // Read the recived data.
        while (rec_val != -1)
        {
            if (data.ID == 0)
            {
                lastProcess_flag = 1;
                break;
            }
            p.ID = data.ID;
            p.Priority = data.Priority;
            p.RunTime = data.RunTime;
            p.ArrTime = data.ArrTime;
            p.PID = 0;
            p.RemaingTime = data.RunTime;
            p.WatingTime = 0;
            if (algorithm == 1) // HPF
            {
                push(process_list, p.Priority, &p);
            }
            else if (algorithm == 2 || algorithm == 3) // SJF or SRTN
            {
                push(process_list, p.RunTime, &p);
            }

            rec_val = msgrcv(msgqid1, &data, sizeof(data) - sizeof(data.mtype), 0, IPC_NOWAIT);
        }

        if (algorithm == 1) // Preemptive HPF
        {

            if (top(process_list) != NULL && (running_process.Priority == -1 || top(process_list)->Priority < running_process.Priority || finished_flag == 1))
            {

                // stop of running_process processes
                if (finished_flag == 0 && running_process.ID != 0)
                {
                    printf("Process ID: %d Stoped\n", running_process.ID);
                    kill(running_process.PID, SIGSTOP);
                    running_process.RemaingTime -= (getClk() - ProStart);
                    running_process.Stoped = getClk();
                    running_process.Running = 0;
                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tstopped\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                    fprintf(fPointer, WriteBuff);
                    push(process_list, running_process.Priority, &running_process);
                }
                else if (finished_flag == 1) // The current process has finished
                {
                    running_process.RemaingTime = 0;
                    running_process.Stoped = getClk();

                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%0.2f\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime, (getClk() - running_process.ArrTime), (float)(getClk() - running_process.ArrTime) / running_process.RunTime);
                    fprintf(fPointer, WriteBuff);
                    averageWTA += (float)((getClk() - running_process.ArrTime) / running_process.RunTime);
                    averageWt += (float)running_process.WatingTime;
                    CPU_UTILIZATION += running_process.RunTime;
                    push(ProFinished, running_process.ID, &running_process);
                    printf("Process ID: %d end\n", running_process.ID);
                }
                // Get the Next process.
                puff = pop(process_list);
                running_process.ID = puff->ID;
                running_process.PID = puff->PID;
                running_process.RunTime = puff->RunTime;
                running_process.ArrTime = puff->ArrTime;
                running_process.Priority = puff->Priority;
                running_process.WatingTime = puff->WatingTime;
                running_process.RemaingTime = puff->RemaingTime;
                running_process.Stoped = puff->Stoped;
                // fork the process if it new
                if (running_process.PID == 0)
                {
                    int pid = fork();
                    if (pid == -1)
                        perror("error in fork");

                    else if (pid == 0)
                    {
                        char BurstTime[5];
                        char arrt[5];
                        sprintf(BurstTime, "%d", running_process.RunTime);
                        printf(arrt, "%d", running_process.ArrTime);
                        execl("./process.out", "process.out", BurstTime, NULL);
                    }
                    else
                    {
                        running_process.PID = pid;
                        running_process.WatingTime += (getClk() - running_process.ArrTime);
                        running_process.Running = 1;
                        printf("Process ID: %d Starts \n", running_process.ID);
                        sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tstarted\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                        fprintf(fPointer, WriteBuff);
                    }
                }
                else // resume it if it old.
                {
                    running_process.WatingTime += (getClk() - running_process.Stoped);
                    kill(running_process.PID, SIGCONT);
                    running_process.Running = 1;
                    printf("Process ID: %d Resume\n", running_process.ID);
                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tresumed\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                    fprintf(fPointer, WriteBuff);
                }

                finished_flag = 0;
                ProStart = getClk();
            }
            else if (top(process_list) == NULL && lastProcess_flag == 1 && finished_flag)
            {
                running_process.RemaingTime = 0;
                running_process.Running = 0;
                running_process.Stoped = getClk();
                push(ProFinished, running_process.ID, &running_process);
                averageWt += (float)running_process.WatingTime;
                averageWTA += (float)((getClk() - running_process.ArrTime) / running_process.RunTime);
                CPU_UTILIZATION += (float)(running_process.RunTime);
                CPU_UTILIZATION = 100 * (CPU_UTILIZATION / (float)getClk());
                averageWt = averageWt / (float)ProFinished->len;
                averageWTA = averageWTA / (float)ProFinished->len;
                sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%0.2f\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime, (getClk() - running_process.ArrTime), (float)(getClk() - running_process.ArrTime) / running_process.RunTime);
                fprintf(fPointer, WriteBuff);
                kill(getppid(), SIGINT);
                break;
            }
        }

        else if (algorithm == 2) // SRTN
        {
            if (top(process_list) != NULL && (running_process.Priority == -1 || top(process_list)->RemaingTime < running_process.RemaingTime || finished_flag == 1))
            {
                if (finished_flag == 0 && running_process.ID != 0)
                {
                    printf("Process ID: %d Stoped\n", running_process.ID);
                    kill(running_process.PID, SIGSTOP);
                    running_process.RemaingTime -= (getClk() - ProStart);
                    running_process.Stoped = getClk();
                    running_process.Running = 0;
                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tstopped\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                    fprintf(fPointer, WriteBuff);
                    push(process_list, running_process.RemaingTime, &running_process);
                }
                else if (finished_flag == 1) // The current process has finished
                {
                    running_process.RemaingTime = 0;
                    running_process.Stoped = getClk();

                    averageWTA += (float)((getClk() - running_process.ArrTime) / running_process.RunTime);
                    averageWt += (float)running_process.WatingTime;
                    CPU_UTILIZATION += running_process.RunTime;
                    push(ProFinished, running_process.ID, &running_process);
                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tstopped\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                    fprintf(fPointer, WriteBuff);
                    printf("Process ID: %d end\n", running_process.ID);
                }
                // Get the Next process.
                puff = pop(process_list);
                running_process.ID = puff->ID;
                running_process.PID = puff->PID;
                running_process.RunTime = puff->RunTime;
                running_process.ArrTime = puff->ArrTime;
                running_process.Priority = puff->Priority;
                running_process.WatingTime = puff->WatingTime;
                running_process.RemaingTime = puff->RemaingTime;
                running_process.Stoped = puff->Stoped;
                // fork the process if it new
                if (running_process.PID == 0)
                {
                    int pid = fork();
                    if (pid == -1)
                        perror("error in fork");

                    else if (pid == 0)
                    {
                        char BurstTime[5];
                        char arrt[5];
                        sprintf(BurstTime, "%d", running_process.RunTime);
                        printf(arrt, "%d", running_process.ArrTime);
                        execl("./process.out", "process.out", BurstTime, NULL);
                    }
                    else
                    {
                        running_process.PID = pid;
                        running_process.WatingTime += (getClk() - running_process.ArrTime);
                        running_process.Running = 1;
                        sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tstarted\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                        fprintf(fPointer, WriteBuff);
                        printf("Process ID: %d Starts \n", running_process.ID);
                    }
                }
                else // resume it if it old.
                {
                    running_process.WatingTime += (getClk() - running_process.Stoped);
                    kill(running_process.PID, SIGCONT);
                    running_process.Running = 1;
                    printf("Process ID: %d Resume\n", running_process.ID);
                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tresumed\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                    fprintf(fPointer, WriteBuff);
                }

                finished_flag = 0;
                ProStart = getClk();
            }
            else if (top(process_list) == NULL && lastProcess_flag == 1 && finished_flag)
            {
                running_process.RemaingTime = 0;
                running_process.Running = 0;
                running_process.Stoped = getClk();
                push(ProFinished, running_process.ID, &running_process);
                averageWt += (float)running_process.WatingTime;
                averageWTA += (float)((getClk() - running_process.ArrTime) / running_process.RunTime);
                CPU_UTILIZATION += (float)(running_process.RunTime);
                CPU_UTILIZATION = 100 * (CPU_UTILIZATION / (float)getClk());
                averageWt = averageWt / (float)ProFinished->len;
                averageWTA = averageWTA / (float)ProFinished->len;
                sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%0.2f\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime, (getClk() - running_process.ArrTime), (float)(getClk() - running_process.ArrTime) / running_process.RunTime);
                fprintf(fPointer, WriteBuff);
                kill(getppid(), SIGINT);
                break;
            }
        }

        else if (algorithm == 3) // SJF
        {
            if (top(process_list) != NULL && (finished_flag == 1 || running_process.ID == 0))
            {
                if (running_process.ID != 0)
                {
                    running_process.RemaingTime = 0;
                    running_process.Running = 0;
                    running_process.Stoped = getClk();
                    printf("Process ID: %d Stoped\n", running_process.ID);
                    averageWTA += (float)((getClk() - running_process.ArrTime) / running_process.RunTime);
                    averageWt += (float)running_process.WatingTime;
                    CPU_UTILIZATION += running_process.RunTime;
                    sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%0.2f\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime, (getClk() - running_process.ArrTime), (float)(getClk() - running_process.ArrTime) / running_process.RunTime);
                    fprintf(fPointer, WriteBuff);

                    push(ProFinished, running_process.ID, &running_process);
                }
                // Get the Next process.
                puff = pop(process_list);
                running_process.ID = puff->ID;
                running_process.PID = puff->PID;
                running_process.RunTime = puff->RunTime;
                running_process.ArrTime = puff->ArrTime;
                running_process.Priority = puff->Priority;
                running_process.WatingTime = puff->WatingTime;
                running_process.RemaingTime = puff->RemaingTime;
                running_process.Stoped = puff->Stoped;
                // fork the process if it new
                if (running_process.PID == 0)
                {
                    int pid = fork();
                    if (pid == -1)
                        perror("error in fork");

                    else if (pid == 0)
                    {
                        char BurstTime[5];
                        char arrt[5];
                        sprintf(BurstTime, "%d", running_process.RunTime);
                        printf(arrt, "%d", running_process.ArrTime);
                        execl("./process.out", "process.out", BurstTime, NULL);
                    }
                    else
                    {
                        running_process.PID = pid;
                        running_process.WatingTime += (getClk() - running_process.ArrTime);
                        running_process.Running = 1;
                        printf("Process ID: %d Starts \n", running_process.ID);
                        sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tstarted\t\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime);
                        fprintf(fPointer, WriteBuff);
                    }
                }

                finished_flag = 0;
                ProStart = getClk();
            }
            else if (top(process_list) == NULL && lastProcess_flag == 1 && finished_flag)
            {
                running_process.RemaingTime = 0;
                running_process.Running = 0;
                running_process.Stoped = getClk();
                push(ProFinished, running_process.ID, &running_process);

                averageWt += (float)running_process.WatingTime;
                averageWTA += (float)((getClk() - running_process.ArrTime) / running_process.RunTime);
                CPU_UTILIZATION += (float)(running_process.RunTime);
                CPU_UTILIZATION = 100 * (CPU_UTILIZATION / (float)getClk());
                averageWt = averageWt / (float)ProFinished->len;
                averageWTA = averageWTA / (float)ProFinished->len;
                sprintf(WriteBuff, "At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%0.2f\n", getClk(), running_process.ID, running_process.ArrTime, running_process.RunTime, running_process.RemaingTime, running_process.WatingTime, (getClk() - running_process.ArrTime), (float)(getClk() - running_process.ArrTime) / running_process.RunTime);
                fprintf(fPointer, WriteBuff);
                kill(getppid(), SIGINT);
                break;
            }
        }

        rec_val = msgrcv(msgqid1, &data, sizeof(data) - sizeof(data.mtype), 0, IPC_NOWAIT);
    }
    wirte_perf_file(CPU_UTILIZATION, averageWTA, averageWt,averageWt);
    fclose(fPointer);
    printf("Sch. Finished\n");
}
