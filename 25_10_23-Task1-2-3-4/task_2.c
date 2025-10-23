#include <stdio.h>
#include <stdlib.h>

struct rr {
    int pno;       // Process number
    int btime;     // Remaining burst time
    int sbtime;    // Original burst time
    int wtime;     // Waiting time
    int lst;       // Last service time
};

int main() {
    struct rr p[10];
    int i, n, ts;           // ts = time slice
    int ptm = 0;            // process time marker (global clock)
    int flag;               
    int twt = 0, ttt = 0;   // total waiting and turnaround times

    printf("\nROUND ROBIN SCHEDULING\n");

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter the time slice: ");
    scanf("%d", &ts);

    printf("Enter burst time for each process:\n");
    for (i = 0; i < n; i++) {
        printf("Process %d: ", i + 1);
        scanf("%d", &p[i].btime);
        p[i].pno = i + 1;
        p[i].sbtime = p[i].btime;  // save original burst time
        p[i].wtime = 0;
        p[i].lst = 0;
    }

    printf("\nGantt Chart:\n");

    do {
        flag = 0;
        for (i = 0; i < n; i++) {
            if (p[i].btime > 0) {
                flag = 1;  // At least one process is not finished

                printf("P%d(%d→", p[i].pno, ptm);

                // Decide how much time to give this process
                int exec_time = (p[i].btime >= ts) ? ts : p[i].btime;
                p[i].btime -= exec_time;
                ptm += exec_time;

                printf("%d)  ", ptm);

                // Update waiting time for other processes
                for (int j = 0; j < n; j++) {
                    if (j != i && p[j].btime > 0)
                        p[j].wtime += exec_time;
                }
            }
        }
    } while (flag == 1);

    printf("\n\nProcess\tBurstTime\tWaitingTime\tTurnAroundTime\n");
    for (i = 0; i < n; i++) {
        int tat = p[i].wtime + p[i].sbtime;
        printf("P%d\t\t%d\t\t%d\t\t%d\n", p[i].pno, p[i].sbtime, p[i].wtime, tat);
        twt += p[i].wtime;
        ttt += tat;
    }

    printf("\nTotal Waiting Time: %d", twt);
    printf("\nAverage Waiting Time: %.2f", (float)twt / n);
    printf("\nTotal Turnaround Time: %d", ttt);
    printf("\nAverage Turnaround Time: %.2f\n", (float)ttt / n);

    return 0;
}
