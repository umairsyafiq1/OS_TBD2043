#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pno;     // Process number
    int pri;     // Priority
    int btime;   // Burst time
    int wtime;   // Waiting time
} sp;

int main() {
    int i, j, n;
    int tbm = 0, totwtime = 0, totttime = 0;
    sp *p, t;

    printf("\nPRIORITY SCHEDULING\n");
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Allocate memory for n processes
    p = (sp*)malloc(n * sizeof(sp));

    printf("Enter burst time and priority for each process:\n");
    for (i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        scanf("%d%d", &p[i].btime, &p[i].pri);
        p[i].pno = i + 1;
        p[i].wtime = 0;
    }

    // Sort processes by priority (lower number = higher priority)
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (p[i].pri > p[j].pri) {
                t = p[i];
                p[i] = p[j];
                p[j] = t;
            }
        }
    }

    printf("\nProcess\tBurstTime\tPriority\tWaitingTime\tTurnAroundTime\n");

    // Calculate waiting and turnaround times
    for (i = 0; i < n; i++) {
        p[i].wtime = tbm;             // Waiting time = total burst time before this process
        tbm += p[i].btime;            // Update total burst time
        int tat = p[i].wtime + p[i].btime; // Turnaround time

        totwtime += p[i].wtime;
        totttime += tat;

        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
               p[i].pno, p[i].btime, p[i].pri, p[i].wtime, tat);
    }

    // Print totals and averages
    printf("\nTotal Waiting Time: %d", totwtime);
    printf("\nAverage Waiting Time: %.2f", (float)totwtime / n);
    printf("\nTotal Turnaround Time: %d", totttime);
    printf("\nAverage Turnaround Time: %.2f\n", (float)totttime / n);

    free(p);
    return 0;
}
