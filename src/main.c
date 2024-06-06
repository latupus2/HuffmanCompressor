#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printUsage() {
    printf("Usage: ./huffman_compressor <command> <input> <output>\n");
    printf("Commands:\n");
    printf("  encode <input.any> <output> - Encode a single file\n");
    printf("  decode <input.huff> <output.any> - Decode a single file\n");
    printf("  encode_dir <input_dir> <output> - Encode all files in a directory\n");
    printf("  decode_pack <input.huffPack> <output_dir> - Decode the archive into a directory\n");
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    const char *command = argv[1];
    const char *input = argv[2];
    const char *output = argv[3];

    if (strcmp(command, "encode") == 0) {
        fileUtilsEncodeFile(input, output);
    } else if (strcmp(command, "decode") == 0) {
        fileUtilsDecodeFile(input, output);
    } else if (strcmp(command, "encode_dir") == 0) {
        encodeDirectory(input, output);
    } else if (strcmp(command, "decode_pack") == 0) {
        decodePack(input, output);
    } else {
        printUsage();
        return 1;
    }

    return 0;
}