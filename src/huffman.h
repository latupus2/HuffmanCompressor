#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>

typedef struct Node {
    unsigned char symbol;
    unsigned int frequency;
    struct Node *left, *right;
} Node;

typedef struct {
    unsigned char symbol;
    char *code;
} CodeTable;

void encodeHuffmanTree(const char *inputFile, const char *outputFile);
void decodeHuffmanTree(const char *inputFile, const char *outputFile);

#endif 