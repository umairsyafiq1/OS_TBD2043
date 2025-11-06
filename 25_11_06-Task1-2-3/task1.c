#include <stdio.h>

int max[100][100];
int alloc[100][100];
int need[100][100];
int avail[100];
int n, r;

void input();
void show();
void calculate();

int main() {
    printf("********** BANKER'S ALGORITHM ************\n\n");
    input();
    show();
    calculate();
    return 0;
}

//-------------------- INPUT FUNCTION ---------------------
void input() {
    int i, j;
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    printf("Enter the number of resource types: ");
    scanf("%d", &r);

    printf("\n--- Enter MAX matrix values ---\n");
    for (i = 0; i < n; i++) {
        printf("Process P%d:\n", i + 1);
        for (j = 0; j < r; j++) {
            printf("  Maximum need of Resource R%d: ", j + 1);
            scanf("%d", &max[i][j]);
        }
    }

    printf("\n--- Enter ALLOCATION matrix values ---\n");
    for (i = 0; i < n; i++) {
        printf("Process P%d:\n", i + 1);
        for (j = 0; j < r; j++) {
            printf("  Resources of type R%d currently allocated: ", j + 1);
            scanf("%d", &alloc[i][j]);
        }
    }

    printf("\n--- Enter AVAILABLE resources ---\n");
    for (j = 0; j < r; j++) {
        printf("Available instances of Resource R%d: ", j + 1);
        scanf("%d", &avail[j]);
    }
}

//-------------------- DISPLAY FUNCTION ---------------------
void show() {
    int i, j;
    printf("\n-------------------------------------------------------------\n");
    printf("Process\t\tAllocation\t\tMax\t\tAvailable\n");
    printf("-------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t\t", i + 1);
        for (j = 0; j < r; j++)
            printf("%d ", alloc[i][j]);
        printf("\t\t");
        for (j = 0; j < r; j++)
            printf("%d ", max[i][j]);
        printf("\t\t");
        if (i == 0) { // only display available once
            for (j = 0; j < r; j++)
                printf("%d ", avail[j]);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------\n");
}

//-------------------- CALCULATION FUNCTION ---------------------
void calculate() {
    int finish[100] = {0}, safeSeq[100];
    int work[100];
    int i, j, k, count = 0;

    // Compute Need Matrix
    for (i = 0; i < n; i++) {
        for (j = 0; j < r; j++) {
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    // Initialize work = available
    for (j = 0; j < r; j++)
        work[j] = avail[j];

    printf("\n--- NEED MATRIX ---\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t", i + 1);
        for (j = 0; j < r; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

    int doneSomething;
    do {
        doneSomething = 0;
        for (i = 0; i < n; i++) {
            if (finish[i] == 0) {
                int canExecute = 1;
                for (j = 0; j < r; j++) {
                    if (need[i][j] > work[j]) {
                        canExecute = 0;
                        break;
                    }
                }

                if (canExecute) {
                    for (k = 0; k < r; k++)
                        work[k] += alloc[i][k];
                    safeSeq[count++] = i;
                    finish[i] = 1;
                    doneSomething = 1;
                }
            }
        }
    } while (doneSomething);

    // Check if system is in safe state
    int safe = 1;
    for (i = 0; i < n; i++) {
        if (finish[i] == 0) {
            safe = 0;
            break;
        }
    }

    if (safe) {
        printf("\n✅ The system is in a SAFE STATE.\n");
        printf("Safe sequence is: ");
        for (i = 0; i < n; i++) {
            printf("P%d", safeSeq[i] + 1);
            if (i < n - 1)
                printf(" -> ");
        }
        printf("\n");
    } else {
        printf("\n⚠️ The system is in an UNSAFE STATE (Deadlock possible).\n");
        printf("Unfinished processes: ");
        for (i = 0; i < n; i++) {
            if (finish[i] == 0)
                printf("P%d ", i + 1);
        }
        printf("\n");
    }
}
