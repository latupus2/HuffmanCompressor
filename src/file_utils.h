#ifndef FILE_UTILS_H
#define FILE_UTILS_H

void fileUtilsEncodeFile(const char *inputFile, const char *outputFile);
void fileUtilsDecodeFile(const char *inputFile, const char *outputFile);
void encodeDirectory(const char *inputDir, const char *outputFile);
void decodePack(const char *inputFile, const char *outputDir);

#endif 