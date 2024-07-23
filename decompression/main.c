#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* filename = NULL;
int originalBytes;
unsigned char symbolsCount;
FILE *input, *output;
int bufforCount = -1;
unsigned char buffor;
int treeCount;

typedef struct Tree {
    int value;
    struct Tree* nextL;
    struct Tree* nextR;
}tree;

void displayTraversal(tree* node){
    if(node == NULL){
        return;
    }
    printf("%i\n", node->value);

    displayTraversal(node->nextL);
    displayTraversal(node->nextR);
}

unsigned char readBit(){
    if(bufforCount == -1){
        fread(&buffor, sizeof(buffor), 1, input);
        bufforCount = 7;
    }

    unsigned char byte = buffor;
    unsigned char bit = (byte >> bufforCount) & 1;
    bufforCount--;
    return bit;
}

void recreateTree(tree** root) {
    unsigned char bit = readBit();

    if (bit == 0) {
        if (*root == NULL) {
            *root = (tree*)malloc(sizeof(tree));
            (*root)->value = -1;
            (*root)->nextL = NULL;
            (*root)->nextR = NULL;
        }

        recreateTree(&((*root)->nextL));
        recreateTree(&((*root)->nextR));
    } else {
        if (*root == NULL) {
            *root = (tree*)malloc(sizeof(tree));
            if (*root == NULL) {
                exit(1);
            }
            (*root)->nextL = NULL;
            (*root)->nextR = NULL;
        }
        unsigned char value = 0;
        for (int j = 0; j < 8; ++j) {
            value <<= 1;
            value |= readBit();
        }
        (*root)->value = value;
    }

    treeCount--;
}

void decompressFile(tree* root){
    while(1) {
        unsigned char bit = readBit();
        if(bit == 0){
            if(root->nextR->value != -1){
                putc((char)root->nextR->value, output);
                break;
            }
            root = root->nextR;
        } else {
            if(root->nextL->value != -1){
                putc((char)root->nextL->value, output);
                break;
            }
            root = root->nextL;
        }
    }
}

void freeTree(tree* node) {
    if (node != NULL) {
        freeTree(node->nextL);
        freeTree(node->nextR);
        free(node);
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("nie podano pliku\n");
        return 1;
    }

    filename = argv[1];
    input = fopen(filename, "rb");

    fread(&originalBytes, sizeof(originalBytes), 1, input);
    fread(&symbolsCount, sizeof(symbolsCount), 1, input);
    treeCount = symbolsCount * 2 - 1;

    tree* root = NULL;
    recreateTree(&root);
//    displayTraversal(root);

    char outputFile[strlen(filename) + 20];
    strcpy(outputFile, filename);
    char *dot = strrchr(outputFile, '.');
    if(dot != NULL) {
        *dot = '\0';
    }
    strcat(outputFile, ".recovery");
    output = fopen(outputFile, "wb");
    for (int i = 0; i < originalBytes; ++i) {
        decompressFile(root);
    }

    freeTree(root);
    return 0;
}
