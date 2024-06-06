#!/bin/bash

gcc -o huffman_compressor src/huffman.c src/file_utils.c src/main.c

if [ $? -eq 0 ]; then
    echo "The compilation completed successfully. Executable file: huffman_compressor"
    
    chmod +x huffman_compressor
    if [ -f huffman_compressor ] && [ -x huffman_compressor ]; then
        echo "The executable has been created and has execute permissions."
    else
        echo "Error: Executable file not found or not authorized to execute."
    fi
else
    echo "Compilation error."
fi