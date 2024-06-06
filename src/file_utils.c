#include "file_utils.h"
#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Проверка существования файла/папки
int fileExists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

// Кодирование одного файла
void fileUtilsEncodeFile(const char *inputFile, const char *outputFile) {
    if (!fileExists(inputFile)) {
        printf("Error: file %s does not exist\n", inputFile);
        return;
    }

    char outputFileWithExt[1024];
    snprintf(outputFileWithExt, sizeof(outputFileWithExt), "%s.huff", outputFile);

    encodeHuffmanTree(inputFile, outputFileWithExt);
    printf("File encoding completed\n");
    
}

// Декодирование одного файла
void fileUtilsDecodeFile(const char *inputFile, const char *outputFile) {
    if (!fileExists(inputFile)) {
        printf("Error: file %s does not exist\n", inputFile);
        return;
    }

    const char *ext = strrchr(inputFile, '.');
    if (!ext || (strcmp(ext, ".huff") != 0)) {
        printf("Error: the input file must have the extension .huff\n");
        return;
    }

    decodeHuffmanTree(inputFile, outputFile);

    printf("File decoding completed\n");
}

// Кодирование всех файлов в папке в один архивный файл
void encodeDirectory(const char *inputDir, const char *outputFile) {
    if (!fileExists(inputDir)) {
        printf("Error: directory %s does not exist\n", inputDir);
        return;
    }

    char outputFileWithExt[1024];
    snprintf(outputFileWithExt, sizeof(outputFileWithExt), "%s.huffPack", outputFile);

    char command[1024];
    snprintf(command, sizeof(command), "ls %s > filelist.txt", inputDir);
    system(command);

    FILE *filelist = fopen("filelist.txt", "r");
    if (filelist == NULL) {
        return;
    }

    FILE *out = fopen(outputFileWithExt, "wb");
    if (out == NULL) {
        fclose(filelist);
        return;
    }

    char filename[256];
    while (fgets(filename, sizeof(filename), filelist)) {
        filename[strcspn(filename, "\n")] = 0;

        char inputFilePath[1024];
        snprintf(inputFilePath, sizeof(inputFilePath), "%s/%s", inputDir, filename);

        // Временный файл для хранения закодированных данных
        char tempEncodedFile[1024];
        snprintf(tempEncodedFile, sizeof(tempEncodedFile), "%s.temp", inputFilePath);

        encodeHuffmanTree(inputFilePath, tempEncodedFile);

        unsigned int nameLength = strlen(filename);
        fwrite(&nameLength, sizeof(nameLength), 1, out);
        fwrite(filename, sizeof(char), nameLength, out);

        FILE *encodedFile = fopen(tempEncodedFile, "rb");
        if (encodedFile == NULL) {
            fclose(out);
            fclose(filelist);
            return;
        }

        fseek(encodedFile, 0, SEEK_END);
        long fileSize = ftell(encodedFile);
        fseek(encodedFile, 0, SEEK_SET);

        fwrite(&fileSize, sizeof(fileSize), 1, out);

        char *buffer = (char*)malloc(fileSize);
        fread(buffer, 1, fileSize, encodedFile);
        fwrite(buffer, 1, fileSize, out);

        free(buffer);
        fclose(encodedFile);

        remove(tempEncodedFile);
    }

    fclose(out);
    fclose(filelist);
    system("rm filelist.txt");

    printf("Files encoding completed\n");
}

// Декодирование архивного файла в указанную папку
void decodePack(const char *inputFile, const char *outputDir) {
    if (!fileExists(inputFile)) {
    printf("Error: file %s does not exist\n", inputFile);
        return;
    }

    const char *ext = strrchr(inputFile, '.');
    if (!ext || (strcmp(ext, ".huffPack") != 0)) {
        printf("Error: the input file must have the extension .huffPack\n");
        return;
    }

    FILE *in = fopen(inputFile, "rb");
    if (in == NULL) {
        return;
    }

    while (!feof(in)) {
        unsigned int nameLength;
        if (fread(&nameLength, sizeof(nameLength), 1, in) != 1) break;

        char filename[256];
        fread(filename, sizeof(char), nameLength, in);
        filename[nameLength] = '\0';

        long fileSize;
        fread(&fileSize, sizeof(fileSize), 1, in);

        char *buffer = (char*)malloc(fileSize);
        fread(buffer, 1, fileSize, in);

        // Временный файл для хранения закодированных данных
        char tempEncodedFile[1024];
        snprintf(tempEncodedFile, sizeof(tempEncodedFile), "%s.temp", filename);

        FILE *tempFile = fopen(tempEncodedFile, "wb");
        if (tempFile == NULL) {
            free(buffer);
            fclose(in);
            return;
        }

        fwrite(buffer, 1, fileSize, tempFile);
        free(buffer);
        fclose(tempFile);

        // Декодируем временный файл в выходную директорию
        char outputFilePath[1024];
        snprintf(outputFilePath, sizeof(outputFilePath), "%s/%s", outputDir, filename);
        decodeHuffmanTree(tempEncodedFile, outputFilePath);

        remove(tempEncodedFile);
    }

    fclose(in);
    printf("Files decoding completed\n");
}