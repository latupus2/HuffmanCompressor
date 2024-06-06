## Основная информация
    Архиватор на основе алгоритма Хаффмана, способный сжимать и разжимать файлы.
    Запуск программы осуществляется через консоль.

## Команды
    encode <input.any> <output> - Кодирует указанный файл в файл .huff.
        <input.any> - Имя файла, который нужно сжать.
        <output> - Имя выходного файла. (расширение .huff добавится автоматически)

    decode <input.huff> <output.any> - Декодирует указанный .huff файл в любой другой файл.
        <input.huff> - Имя файла, который нужно разжать. (файл обязан быть .huff)
        <output.any> - Имя выходного файла. (расширение нужно указать самостоятельно) 

    encode_dir <input_dir> <output> - Кодирует все элементы указанной папки в один файл .huffPack.
        <input_dir> - Папка, все файлы которой будут сжаты в один файл .huffPack.
        <output> - Имя выходного файла. (расширение .huffPack добавится автоматически)

    decode_pack <input.huffPack> <output_dir> - Декодирует файл .huffPack в изначальные файлы, и помещает их в указанную папку.
        <input.huffPack> - Имя файла, который нужно разжать. (файл обязан быть .huffPack)
        <output_dir> - Папка, в которую будут помещены расжатые файлы.

## Примеры использования
    ./huffman_comressor encode MyText.txt compressedText
        - Закодирует файл MyText в файл compressedText.huff

    ./huffman_comressor decode compressedText.huff MyText.txt
        - Декодирует файл compressedText.huff в файл MyText.txt

    ./huffman_comressor encode_dir MyFiles files
        - Закодирует все файлы в папке MyFiles в файл files.huffPack

    ./huffman_comressor decode_pack files.huffPack MyFiles
        - Декодирует файл files.huffPack и переместит полученные файлы в папку MyFiles