#include <stdio.h>

int main() 
{
    int q[20], p[50], c = 0;
    int c1, f, i, j, k = 0, n, r;
    int b[20], c2[20], t;

    printf("Enter no of pages: ");
    scanf("%d", &n);

    printf("Enter the reference string: ");
    for(i = 0; i < n; i++)
        scanf("%d", &p[i]);

    printf("Enter no of frames: ");
    scanf("%d", &f);

    // Load first page
    q[k] = p[k];
    printf("\n\t%d\n", q[k]);
    c++;
    k++;

    // Process each page
    for(i = 1; i < n; i++) {

        c1 = 0;
        for(j = 0; j < f; j++) {
            if(p[i] != q[j])
                c1++;
        }

        // Page fault if not present in any frame
        if(c1 == f) {
            c++;

            // If free frame available
            if(k < f) {
                q[k] = p[i];
                k++;

                for(j = 0; j < k; j++)
                    printf("\t%d", q[j]);
                printf("\n");
            }
            else {
                // Compute LRU distances
                for(r = 0; r < f; r++) {
                    c2[r] = 0;

                    // Scan backwards
                    for(j = i - 1; j >= 0; j--) {
                        if(q[r] != p[j])
                            c2[r]++;
                        else
                            break;
                    }
                }

                // Copy distances for sorting
                for(r = 0; r < f; r++)
                    b[r] = c2[r];

                // Sort descending to find max distance
                for(r = 0; r < f; r++) {
                    for(j = r; j < f; j++) {
                        if(b[r] < b[j]) {
                            t = b[r];
                            b[r] = b[j];
                            b[j] = t;
                        }
                    }
                }

                // Replace LRU frame
                for(r = 0; r < f; r++) {
                    if(c2[r] == b[0]) {
                        q[r] = p[i];
                        break;
                    }
                }

                // Print frames
                for(r = 0; r < f; r++)
                    printf("\t%d", q[r]);

                printf("\n");
            }
        }
    }

    printf("\nTotal number of page faults = %d\n", c);

    return 0;
}
