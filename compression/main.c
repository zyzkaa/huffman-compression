#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ELEMENTS 256

int byteCount = 0;
int shannonModel[ELEMENTS] = {0};
int sortedShannonModel[ELEMENTS][2];
int symbolsCount = 0;
char *pathName = NULL;
unsigned char bitBuffer = 0;
int bitCount = 0;
FILE* comp;
int outByteCount = 0;

typedef struct Tree {
    int value;
    int quantity;
    struct Tree* nextL;
    struct Tree* nextR;
}tree;

typedef struct Word{
    int value;
    short int bitsAmount;
    unsigned char bit[32];
} word;

void makeFileName(char output[], char ext[]){
    char fileName[strlen(pathName)];
    strcpy(fileName, pathName);
    char *dot = strrchr(fileName, '.');
    if(dot != NULL) {
        *dot = '\0';
    }
    strcpy(output, fileName);
    strcat(output, ext);
}

void makeModel() {
    FILE *file = fopen(pathName, "rb");
    if (file == NULL) {
        printf("podany argument nie jest plikiem\n");
        return;
    }
    unsigned char byte;
    while (fread(&byte, sizeof(byte), 1, file) == 1) {
        shannonModel[byte]++;
        byteCount++;
    }
    fclose(file);
}

void sortModel() {
    for (int i = 0; i < ELEMENTS; i++) {
        sortedShannonModel[i][0] = i;
        sortedShannonModel[i][1] = shannonModel[i];
    }

    for (int i = 0; i < ELEMENTS - 1; i++) {
        int max_idx = i;
        for (int j = i + 1; j < ELEMENTS; j++) {
            if (sortedShannonModel[j][1] > sortedShannonModel[max_idx][1]) {
                max_idx = j;
            }
        }
        int temp_val = sortedShannonModel[max_idx][1];
        int temp_index = sortedShannonModel[max_idx][0];
        sortedShannonModel[max_idx][1] = sortedShannonModel[i][1];
        sortedShannonModel[max_idx][0] = sortedShannonModel[i][0];
        sortedShannonModel[i][1] = temp_val;
        sortedShannonModel[i][0] = temp_index;
    }
}

void printModels(){
    printf("model Shannona nieposortowany:\n");
    for (int i = 0; i < ELEMENTS; i++) {
        if (shannonModel[i] != 0) {
            printf("%d %d\n", i, shannonModel[i]);
        }
    }
    printf("\n");

    printf("model Shannona posortowany:\n");
    for (int i = 0; i < ELEMENTS; i++) {
        if (sortedShannonModel[i][1] != 0) {
            printf("%d %d\n", sortedShannonModel[i][0], sortedShannonModel[i][1]);
        }
    }
    printf("\n");
}

void saveModels() {
    int length = (int)strlen(pathName);
    char fileName[length];
    strcpy(fileName, pathName);
    char *dot = strrchr(fileName, '.');
    if(dot != NULL) {
        *dot = '\0';
    }

    char modelFile[length + 15], sortedModelFile[length + 15], byteFile[length + 15];
    makeFileName(modelFile, ".model");
    makeFileName(sortedModelFile, ".modelSort");
    makeFileName(byteFile, ".ileBajtow");

    FILE *file;
    file = fopen(byteFile, "w");
    fprintf(file, "%d", byteCount);
    fclose(file);

    file = fopen(modelFile, "w");
    for (int i = 0; i < ELEMENTS; i++) {
        if (shannonModel[i] != 0) {
            fprintf(file, "%d %d\n", i, shannonModel[i]);
        }
    }
    fclose(file);

    file = fopen(sortedModelFile, "w");
    for (int i = 0; i < ELEMENTS; i++) {
        if (sortedShannonModel[i][1] != 0) {
            fprintf(file, "%c %d %d\n", sortedShannonModel[i][0], sortedShannonModel[i][0], sortedShannonModel[i][1]);
        }
    }
    fclose(file);
}

void countSymbols(){
    for (int i = 0; i < ELEMENTS; ++i) {
        if(shannonModel[i]!= 0){
            symbolsCount++;
        }
    }
};

void printNode(tree* node){
    if(node->nextL != NULL && node->nextR != NULL){
        printf("%i %i %i \n", node->value, node->nextL->value, node->nextR->value);

    } else {
        printf("%i \n", node->value);
    }
}

void printTreeHelper(tree* node, int level) {
    if (node == NULL) {
        return;
    }

    printTreeHelper(node->nextR, level + 1);
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d\n", node->value);
    printTreeHelper(node->nextL, level + 1);
}

void printTreeVisually(tree* root) {
    printTreeHelper(root, 0);
}

void repairQueue(tree** queue, int start){
    tree* temp;
    for (int i = start; i < symbolsCount-1; ++i) {
        if(queue[i]->quantity > queue[i+1]->quantity){
            temp = queue[i];
            queue[i] = queue[i+1];
            queue[i+1] = temp;
        } else {
            break;
        }
    }
};

void makeAndSaveTree(tree** root) {
    tree* queue[symbolsCount];

    for (int i = 0; i < symbolsCount; ++i) {
        tree* leaf = (tree*) malloc(sizeof (tree));
        leaf->value = sortedShannonModel[symbolsCount-i-1][0];
        leaf->quantity = sortedShannonModel[symbolsCount-i-1][1];
        leaf->nextL = leaf->nextR = NULL;
        queue[i] = leaf;
    }

    char treeFile[strlen(pathName)];
    makeFileName(treeFile, ".drzewo");

    FILE *file;
    file = fopen(treeFile, "w");
    int queueStart = 0;
    for (int i = 0; i < symbolsCount-1; ++i) {
        tree *newNode = (tree *) malloc(sizeof(tree));
        newNode->value = 256+queueStart;
        newNode->nextL = queue[queueStart];
        newNode->nextR = queue[++queueStart];
        newNode->quantity = newNode->nextL->quantity + newNode->nextR->quantity;
        queue[queueStart] = newNode;
        printNode(newNode);
        fprintf(file, "%i %i %i\n", newNode->value, newNode->nextL->value, newNode->nextR->value);
        repairQueue(queue, queueStart);
    }
    fclose(file);
    *root = queue[queueStart];
}

void freeTree(tree* node) {
    if (node != NULL) {
        freeTree(node->nextL);
        freeTree(node->nextR);
        free(node);
    }
}

void makeFullDictionary(word fullD[], tree* root, int size){
    tree* queue[size];
    queue[0] = root;
    int queueEnd = 1;

    for (int i = 0; i < size; ++i) {
        fullD[i].value = 0;
        fullD[i].bitsAmount = 0;
        for (int j = 0; j < 32; ++j) {
            fullD[i].bit[j] = 0;
        }
    }

    for (int i = 0; i < size; ++i) {
        fullD[i].value = queue[i]->value;
        if(queue[i]->nextL != NULL){
            fullD[queueEnd].bitsAmount = fullD[i].bitsAmount + 1;
            for (int j = 0; j < fullD[queueEnd].bitsAmount; ++j) {
                fullD[queueEnd].bit[j] = fullD[i].bit[j];
            }
            fullD[queueEnd].bit[fullD[queueEnd].bitsAmount - 1] = 1;
            queue[queueEnd] = queue[i]->nextL;
            queueEnd++;
        }
        if(queue[i]->nextR != NULL){
            fullD[queueEnd].bitsAmount = fullD[i].bitsAmount + 1;
            for (int j = 0; j < fullD[queueEnd].bitsAmount; ++j) {
                fullD[queueEnd].bit[j] = fullD[i].bit[j];
            }
            queue[queueEnd] = queue[i]->nextR;
            queueEnd++;
        }
    }
}

void makeDictionaryFromFull(word fullD[], word dictionary[]){
    int index = 0;
    for (int i = 0; i < symbolsCount * 2; ++i) {
        if(fullD[i].value < 256){
            dictionary[index++] = fullD[i];
        }
    }
}

void printDictionary(word dictionary[], int size){
    int sum;
    for (int i = 0; i < size; ++i) {
        printf("%i %i ", dictionary[i].value, dictionary[i].bitsAmount);
        for (int j = 0; j < 32; j+=8) {
            sum = 0;
            for (int k = 0; k < 8; ++k) {
                if(dictionary[i].bit[j+k] == 1){
                    sum += pow(2, 7-k);
                }
            }
            printf("%i ", sum);
        }
        printf("\n");
    }
    printf("\n");
}

void saveDictionaries(word full[], int fullSize, word final[], int finalSize){
    int length = (int)strlen(pathName);
    char fileName[length];
    strcpy(fileName, pathName);
    char *dot = strrchr(fileName, '.');
    if(dot != NULL) {
        *dot = '\0';
    }

    char fullDictionaryFile[length + 15], dictionaryFile[length + 15];
    makeFileName(fullDictionaryFile, ".tabelaKoduFull");
    makeFileName(dictionaryFile, ".tabelaKodu");

    int sum;
    FILE* file;
    file = fopen(fullDictionaryFile, "w");
    for (int i = 0; i < fullSize; ++i) {
        fprintf(file,"%i %i ", full[i].value, full[i].bitsAmount);
        for (int j = 0; j < 32; j+=8) {
            sum = 0;
            for (int k = 0; k < 8; ++k) {
                if(full[i].bit[j+k] == 1){
                    sum += pow(2, 7-k);
                }
            }
            fprintf(file, "%i ", sum);
        }
        fprintf(file,"\n");
    }
    fclose(file);

    file = fopen(dictionaryFile, "w");
    for (int i = 0; i < finalSize; ++i) {
        fprintf(file,"%i %i ", final[i].value, final[i].bitsAmount);
        for (int j = 0; j < 32; j+=8) {
            sum = 0;
            for (int k = 0; k < 8; ++k) {
                if(final[i].bit[j+k] == 1){
                    sum += pow(2, 7-k);
                }
            }
            fprintf(file, "%i ", sum);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

void addToBufor(unsigned char bit){
    if(bitCount < 8){
        bitCount++;
        bitBuffer = bitBuffer << 1;
        bitBuffer = bitBuffer | bit;
    }
    if(bitCount == 8){
        fwrite(&bitBuffer, sizeof(bitBuffer), 1, comp);
        outByteCount++;
        bitCount = 0;
        bitBuffer = 0;
    }

}

void preorderTraversal(tree* node){
    if(node == NULL){
        return;
    }

    if(node->value > 255){
        addToBufor(0);
    } else {
        addToBufor(1);
        unsigned char value;
        for (int i = 0; i < 8; ++i) {
            value = node->value;
            value = value >> (7 - i);
            unsigned char lastBit = 1;
            lastBit = value & lastBit;
            addToBufor(lastBit);
        }
    }
    preorderTraversal(node->nextL);
    preorderTraversal(node->nextR);
}

void saveInitialInfo(tree* root){
    fwrite(&byteCount, sizeof(byteCount), 1, comp);
    byteCount += 2;

    unsigned char symbols = (unsigned char)symbolsCount;
    fwrite(&symbols, sizeof(symbols), 1, comp);
    byteCount++;

    preorderTraversal(root);
}

void compressFile(word dictionary[]){
    FILE* input = fopen(pathName, "rb");
    unsigned char byte;
    while (fread(&byte, sizeof(byte), 1, input) == 1) {
        for (int j = 0; j < symbolsCount; ++j) {
            if(dictionary[j].value == byte){
                for (int k = 0; k < dictionary[j].bitsAmount; ++k) {
                    addToBufor(dictionary[j].bit[k]);
                }
            }
        }
    }

    if(bitCount != 0){
        for (int i = bitCount; i < 8; ++i) {
            bitBuffer = bitBuffer << 1;
        }
        fwrite(&bitBuffer, sizeof(bitBuffer), 1, comp);
        bitCount = 0;
        bitBuffer = 0;
        outByteCount++;
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("nie podano pliku\n");
        return 1;
    }

    pathName = argv[1];
    tree* root = NULL;

    FILE* file;
    if ((file = fopen(pathName, "r")) == NULL) {
        printf("nie ma takiego pliku\n");
        return 0;
    }
    fclose(file);


    makeModel();
    sortModel();
    countSymbols();
    printf("\nprzeczytano %d bajtow\n\n", byteCount);
    printModels();
    saveModels();

    printf("drzewo Huffmana:\n");
    makeAndSaveTree(&root);
    printf("\n");

    int fullSize = symbolsCount * 2 - 1;
    word fullDictionary[fullSize];
    word dictionary[symbolsCount];
    makeFullDictionary(fullDictionary, root, fullSize);
    printf("pelna tabela kodu:\n");
    printDictionary(fullDictionary, fullSize);
    makeDictionaryFromFull(fullDictionary, dictionary);
    printf("tabela kodu:\n");
    printDictionary(dictionary, symbolsCount);
    saveDictionaries(fullDictionary, fullSize, dictionary, symbolsCount);


    char compFile[strlen(pathName) + 15];
    makeFileName(compFile, ".Huffman");
    comp = fopen(compFile, "wb");
    saveInitialInfo(root);
    printf("\n");
    compressFile(dictionary);
    fclose(comp);

    printf("do pliku skompresowanego zapisano %i bajtow \n", outByteCount);
    float indicator = ((float)outByteCount / (float)byteCount) * 100;
    printf("wskaznik upakowania %.1f procent\n", indicator);
    freeTree(root);
    return 0;
}
