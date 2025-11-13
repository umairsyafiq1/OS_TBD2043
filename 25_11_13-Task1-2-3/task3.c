#include <stdio.h>
#define MAX 25

int main(){
    int frag[MAX], b[MAX], f[MAX], bf[MAX] = {0}, ff[MAX] = {0};
    int nb, nf, temp, lowest;

    printf("\n\tMemory Management Scheme - Best Fit\n");

    printf("Enter the number of blocks: ");
    scanf("%d", &nb);

    printf("Enter the number of files: ");
    scanf("%d", &nf);

    printf("\nEnter the size of the blocks:\n");
    for (int i = 0; i < nb; i++){
        printf("Block %d: ", i + 1);
        scanf("%d", &b[i]);
    }

    printf("\nEnter the size of the files:\n");
    for (int i = 0; i < nf; i++){
        printf("File %d: ", i + 1);
        scanf("%d", &f[i]);
    }

    // Best Fit Allocation
    for (int i = 0; i < nf; i++){
        lowest = 10000; // Reset for each file
        ff[i] = -1;     // Initially not allocated
  
        for (int j = 0; j < nb; j++){
            if (bf[j] == 0){
                temp = b[j] - f[i];
                if (temp >= 0 && temp < lowest){
                    ff[i] = j;
                    lowest = temp;
                }
            }
        }

        if (ff[i] != -1){
            frag[i] = lowest;
            bf[ff[i]] = 1;
        }
        else{
            frag[i] = -1; // No suitable block found
        }
    }

    printf("\nFile_no\tFile_size\tBlock_no\tBlock_size\tFragment\n");
    for (int i = 0; i < nf; i++){
        if (ff[i] != -1){
            printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
                   i + 1, f[i], ff[i] + 1, b[ff[i]], frag[i]);
        }
        else{
            printf("%d\t\t%d\t\tNot Allocated\t-\t\t-\n", i + 1, f[i]);
        }
    }

    return 0;
}
