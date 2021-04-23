#include "image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
    Peter Zhang
    11/8/2019
    PGM Image decoder/encoder
    This program has five functions that will work together with the code in
    main.c to encode and decode pgm images.
*/
void resetArray(int *arr, int num) {
    for (int i = 0; i < num; i++) {
        arr[i] = 0;
    }
}
char convertFromBinary(int *arr) {
    int total = 0;
    int binaryVals[8] = {128, 64, 32, 16, 8, 4, 2, 1};
    for (int i = 0; i < 8; i++) {
        total += binaryVals[i] * arr[i];
    }
    // printf("%c\n", total);
    return total;
}

// void convertToBinary(char letter, int binaryOutput[])
// {
//     int i = 0;
//     while (i <= 7)
//     {
//         binaryOutput[i] = letter % 2;
//         letter /= 2;
//         i++;
//     }
// }

void printArray(int *arr, int num) {
    for (int i = 0; i < num; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
int getBit(int n, int bit) {
    for (int i = 0; i < bit; i++) {
        n = n / 2;
    }
    return n % 2;
}

// width = cols, height = rows
ImagePGM *readPGM(char *filename) {
    FILE *fileToOpen;
    fileToOpen = fopen(filename, "r");
    if (fileToOpen == NULL) return NULL;
    ImagePGM *PGMFILE = malloc(sizeof(ImagePGM));
    // fgets(PGMFILE->magic, 3, fileToOpen);
    fscanf(fileToOpen, "%s", PGMFILE->magic);
    fscanf(fileToOpen, "%d%d%d", &PGMFILE->width, &PGMFILE->height,
           &PGMFILE->max_value);
    // printf("%d %d %d\n", PGMFILE->height, PGMFILE->width,
    // PGMFILE->max_value);
    PGMFILE->pixels = (int **)malloc(sizeof(int *) * PGMFILE->height);
    // printf("malloced array with %d\n", PGMFILE->height);
    for (int i = 0; i < PGMFILE->height; i++) {
        PGMFILE->pixels[i] = (int *)malloc(sizeof(int) * PGMFILE->width);
        // printf("malloced col %d with %d\n",i ,PGMFILE->height);
    }
    for (int i = 0; i < PGMFILE->height; i++) {
        // printf("im here\n");
        for (int j = 0; j < PGMFILE->width; j++) {
            fscanf(fileToOpen, "%d", &PGMFILE->pixels[i][j]);
            // printf("pixels[%d][%d]=%d ",i,j, PGMFILE->pixels[i][j]);
            // printf("scanned in %d to pixels[%d][%d]\n",
            // PGMFILE->pixels[i][j], i, j);
        }
        // printf("\n");
    }
    fclose(fileToOpen);
    return PGMFILE;
}

int writePGM(ImagePGM *pImagePGM, char *filename) {
    FILE *fileToOpen;
    fileToOpen = fopen(filename, "w");
    if (fileToOpen == NULL) return 0;
    fprintf(fileToOpen, "%s\n", pImagePGM->magic);
    fprintf(fileToOpen, "%d %d %d\n", pImagePGM->width, pImagePGM->height,
            pImagePGM->max_value);
    for (int i = 0; i < pImagePGM->height; i++) {
        for (int j = 0; j < pImagePGM->width; j++) {
            fprintf(fileToOpen, "%d ", pImagePGM->pixels[i][j]);
        }
        fprintf(fileToOpen, "\n");
    }
    return 1;
}

int encode(ImagePGM *pImagePGM, char *msg) {
    int len = strlen(msg) + 1;
    if (len * 8 > pImagePGM->height * pImagePGM->width) return 0;  // too small
    int binaryNum, binaryNum2, r = pImagePGM->height - 1,
                               c = pImagePGM->width - 1;
    for (int i = 0; i <= strlen(msg); i++) {
        binaryNum2 = msg[i];
        for (int j = 7; j >= 0; j--) {
            if (c < 0) {
                r--;
                c = pImagePGM->width - 1;
            }
            binaryNum = getBit(binaryNum2, j);
            // printf("%d ", binaryNum);
            // binaryNum2 /= 2;
            if (binaryNum == pImagePGM->pixels[r][c] % 2) {
                // do nothing
            } else {
                if (pImagePGM->pixels[r][c] % 2 == 0)
                    pImagePGM->pixels[r][c]++;
                else if (pImagePGM->pixels[r][c] % 2 == 1)
                    pImagePGM->pixels[r][c]--;
            }
            c--;
        }
        // printf("\n");
    }
    return 1;
}

char *decode(ImagePGM *pImagePGM) {
    int sizeOfMsg = pImagePGM->height * pImagePGM->width / 8;
    char *msg = (char *)malloc(sizeof(char) * sizeOfMsg);
    int arr[8] = {0};
    // printArray(arr, 8);
    int count = 0, msgCount = 0;
    for (int i = pImagePGM->height - 1; i >= 0; i--) {
        for (int j = pImagePGM->width - 1; j >= 0; j--) {
            if (count > 7) {
                msg[msgCount] = convertFromBinary(arr);
                if (msg[msgCount] == 0) return msg;
                // printArray(arr, 8);
                msgCount++;
                count = 0;
                resetArray(arr, 8);
                // printArray(arr, 8);
            }
            arr[count] = pImagePGM->pixels[i][j] % 2;
            count++;
        }
    }
    return msg;
}

void freePGM(ImagePGM *pImagePGM) {
    for (int i = 0; i < pImagePGM->height; i++) {
        free(pImagePGM->pixels[i]);
    }
    free(pImagePGM->pixels);
    free(pImagePGM);
}
