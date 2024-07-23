# about
Simple Huffman compression and decompression project in C.

# compression
Compressing program reads the input file, constructs and sorts a Shannon model, builds a Huffman tree, and generates corresponding coding tables. Finally, it compresses the input file using the Huffman codes and saves the compressed data to a new file.

### Outputs:
1. Unsorted Shannon model (.model)
2. Sorted Shannon model (.modelSort)
3. Byte count (.ileBajtow)
4. Huffman tree (.drzewo)
5. Full coding table (.tabelaKoduFull)
6. Final coding table (.tabelaKodu)
7. Compressed file (.Huffman)

# decompression
To decompress program reads .Huffman file. It reads the compressed file, reconstructs the Huffman tree, and then uses this tree to decode the compressed data back to its original form.
