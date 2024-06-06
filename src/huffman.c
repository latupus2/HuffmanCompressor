#include "huffman.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_SYMBOLS 256

Node* createNode(unsigned char symbol, unsigned int frequency) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

// Сравнение узлов для сортировки
int compareNodes(const void *a, const void *b) {
    Node *nodeA = *(Node**)a;
    Node *nodeB = *(Node**)b;
    if (nodeA->frequency == nodeB->frequency) {
        return (int)nodeA->symbol - (int)nodeB->symbol;
    }
    return nodeA->frequency - nodeB->frequency;
}

void freeTree(Node *root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void freeCodeTable(CodeTable *table) {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (table[i].code != NULL) {
            free(table[i].code);
            table[i].code = NULL;
        }
    }
}

// Построение дерева Хаффмана
Node* buildTree(unsigned int freq[]) {
    Node *nodes[MAX_SYMBOLS];
    int nodeCount = 0;

    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            nodes[nodeCount++] = createNode((unsigned char)i, freq[i]);
        }
    }

    if (nodeCount == 1) {
        nodes[nodeCount++] = createNode(0, 0);
    }

    while (nodeCount > 1) {
        qsort(nodes, nodeCount, sizeof(Node*), compareNodes);

        Node *left = nodes[0];
        Node *right = nodes[1];

        Node *newNode = createNode(0, left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        nodes[0] = newNode;
        nodes[1] = nodes[nodeCount - 1];
        nodeCount--;
    }

    return nodes[0];
}

// Создание кодовой таблицы
void createCodeTable(Node *root, char *code, int depth, CodeTable table[]) {
    if (root->left == NULL && root->right == NULL) {
        table[root->symbol].symbol = root->symbol;
        table[root->symbol].code = (char*)malloc(depth + 1);
        strncpy(table[root->symbol].code, code, depth);
        table[root->symbol].code[depth] = '\0';
        return;
    }

    if (root->left != NULL) {
        code[depth] = '0';
        createCodeTable(root->left, code, depth + 1, table);
    }

    if (root->right != NULL) {
        code[depth] = '1';
        createCodeTable(root->right, code, depth + 1, table);
    }
}

// Сохранение дерева Хаффмана в файл
void saveTree(Node *root, FILE *out) {
    if (root->left == NULL && root->right == NULL) {
        fputc('1', out);
        fputc(root->symbol, out);
    } else {
        fputc('0', out);
        saveTree(root->left, out);
        saveTree(root->right, out);
    }
}

// Загрузка дерева Хаффмана из файла
Node* loadTree(FILE *in) {
    if (feof(in)) {
        return NULL;
    }
    unsigned char bit = fgetc(in);
    if (bit == '1') {
        unsigned char symbol = fgetc(in);
        return createNode(symbol, 0);
    } else {
        Node *node = createNode(0, 0);
        node->left = loadTree(in);
        node->right = loadTree(in);
        return node;
    }
}

// Запись битов в файл
void writeBit(FILE *out, unsigned char bit, unsigned char *bitBuffer, int *bitCount) {
    *bitBuffer = (*bitBuffer << 1) | bit;
    (*bitCount)++;
    if (*bitCount == 8) {
        fputc(*bitBuffer, out);
        *bitBuffer = 0;
        *bitCount = 0;
    }
}

// Запись кода символа в файл
void writeCode(FILE *out, const char *code, unsigned char *bitBuffer, int *bitCount) {
    while (*code) {
        writeBit(out, *code - '0', bitBuffer, bitCount);
        code++;
    }
}

void finalizeBitBuffer(FILE *out, unsigned char *bitBuffer, int bitCount) {
    if (bitCount > 0) {
        *bitBuffer <<= (8 - bitCount);
        fputc(*bitBuffer, out);
    }
}

// Кодирование файла
void encodeFile(const char *inputFile, const char *outputFile, Node *root, CodeTable table[]) {
    FILE *in = fopen(inputFile, "rb");
    if (!in) {
        printf("Error opening input file (encodeFile)");
        return;
    }
    
    fseek(in, 0, SEEK_END);
    unsigned int fileSize = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    FILE *out = fopen(outputFile, "wb");
    if (!out) {
        printf("Error opening output file (encodeFile)");
        fclose(in);
        return;
    }
    
    // Запись размера исходного файла в начало выходного файла
    fwrite(&fileSize, sizeof(fileSize), 1, out);
    
    saveTree(root, out);
    
    unsigned char buffer[1];
    unsigned char bitBuffer = 0;
    int bitCount = 0;
    
    while (fread(buffer, 1, 1, in)) {
        writeCode(out, table[buffer[0]].code, &bitBuffer, &bitCount);
    }
    
    finalizeBitBuffer(out, &bitBuffer, bitCount);
    
    fclose(in);
    fclose(out);
}

// Чтение битов из файла
int readBit(FILE *in, unsigned char *bitBuffer, int *bitCount) {
    if (*bitCount == 0) {
        if (feof(in)) {
            return EOF;
        }
        int byte = fgetc(in);
        if (byte == EOF) {
            return EOF;
        }
        *bitBuffer = (unsigned char)byte;
        *bitCount = 8;
    }

    int bit = (*bitBuffer >> 7) & 1;
    *bitBuffer <<= 1;
    (*bitCount)--;
    return bit;
}

// Декодирование файла
void decodeFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    if (!in) {
        printf("Error opening input file (decodeFile)");
        return;
    }

    unsigned int fileSize;
    if (fread(&fileSize, sizeof(fileSize), 1, in) != 1) {
        if (feof(in)) {
            printf("Error Unexpected end of file when reading file size (decodeFile)\n");
        }
  
        fclose(in);
        return;
    }

    Node *root = loadTree(in);
    if (!root) {
        printf("Error loading Huffman tree (decodeFile)\n");
        fclose(in);
        return;
    }

    FILE *out = fopen(outputFile, "wb");
    if (!out) {
        printf("Error: opening input file (decodeFile)");
        freeTree(root);
        fclose(in);
        return;
    }

    Node *current = root;
    unsigned char bitBuffer = 0;
    int bitCount = 0;
    int bit;
    unsigned int decodedSymbols = 0;

    while (decodedSymbols < fileSize) {
        bit = readBit(in, &bitBuffer, &bitCount);
        if (bit == EOF) {
            printf("Error: Unexpected end of file during decoding (decodeFile)\n");
            break;
        }

        current = (bit == 1) ? current->right : current->left;

        if (current->left == NULL && current->right == NULL) {
            fputc(current->symbol, out);
            current = root;
            decodedSymbols++;
        }
    }

    fclose(out);
    freeTree(root);
    fclose(in);
}

Node* buildHuffmanTreeFromFile(const char *inputFile) {
    unsigned int freq[MAX_SYMBOLS] = {0};

    FILE *in = fopen(inputFile, "rb");
    unsigned char buffer[1];
    while (fread(buffer, 1, 1, in) > 0) {
        freq[buffer[0]]++;
    }
    fclose(in);

    return buildTree(freq);
}

void encodeHuffmanTree(const char *inputFile, const char *outputFile) {
    Node *root = buildHuffmanTreeFromFile(inputFile);

    CodeTable table[MAX_SYMBOLS] = {0};
    char code[MAX_SYMBOLS];
    createCodeTable(root, code, 0, table);

    encodeFile(inputFile, outputFile, root, table);

    freeTree(root);
    freeCodeTable(table);
}

void decodeHuffmanTree(const char *inputFile, const char *outputFile) {

    decodeFile(inputFile, outputFile);

}

