#include <stdio.h>
#include <stdlib.h>

struct fcfs {
    int pid;    // Process ID
    int btime;  // Burst Time
    int wtime;  // Waiting Time
    int ttime;  // Turnaround Time
} p[10];

int main() {
    int i, n;
    int totwtime = 0, totttime = 0;

    printf("\nFCFS Scheduling...\n");
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Enter burst time for Process %d: ", p[i].pid);
        scanf("%d", &p[i].btime);
    }

    // Calculate waiting time and turnaround time
    p[0].wtime = 0;  // First process waits 0 time
    p[0].ttime = p[0].btime;
    totttime += p[0].ttime;

    for (i = 1; i < n; i++) {
        p[i].wtime = p[i - 1].wtime + p[i - 1].btime;
        p[i].ttime = p[i].wtime + p[i].btime;

        totwtime += p[i].wtime;
        totttime += p[i].ttime;
    }

    printf("\nProcess\tBurstTime\tWaitingTime\tTurnAroundTime\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t\t%d\t\t%d\t\t%d\n",
               p[i].pid, p[i].btime, p[i].wtime, p[i].ttime);
    }

    printf("\nTotal Waiting Time: %d", totwtime);
    printf("\nAverage Waiting Time: %.2f", (float)totwtime / n);
    printf("\nTotal Turnaround Time: %d", totttime);
    printf("\nAverage Turnaround Time: %.2f\n", (float)totttime / n);

    return 0;
}
