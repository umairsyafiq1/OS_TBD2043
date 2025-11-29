#include <stdio.h>

int main() 
{
    int i = 0, j, k = 0, i1 = 0;
    int m, n;
    int rs[30], p[30];
    int flag;

    printf("FIFO Page Replacement Algorithm\n");

    printf("Enter the number of frames: ");
    scanf("%d", &n);

    printf("Enter the reference string (end with 0): ");

    // Read reference string
    while (1) {
        scanf("%d", &rs[i]);
        if (rs[i] == 0)
            break;
        i++;
    }

    m = i;

    // Initialize frames
    for (j = 0; j < n; j++)
        p[j] = -1;

    printf("\nProcessing...\n\n");

    for (i = 0; i < m; i++) {
        flag = 1;

        // Check if page already exists
        for (j = 0; j < n; j++) {
            if (p[j] == rs[i]) {
                printf("Page %d already in frame.\n", rs[i]);
                flag = 0;
                break;
            }
        }

        // If not present -> page fault
        if (flag == 1) {
            p[i1] = rs[i];
            i1 = (i1 + 1) % n;
            k++;

            printf("Page fault → Loaded %d into frames:\n", rs[i]);
            for (j = 0; j < n; j++) {
                printf("Frame %d: %d", j + 1, p[j]);
                if (p[j] == rs[i])
                    printf("  <- inserted");
                printf("\n");
            }
            printf("\n");
        }
    }

    printf("Total number of page faults = %d\n", k);

    return 0;
}
