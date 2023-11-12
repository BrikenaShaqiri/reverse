#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 3)
    {
        printf("Usage: reverse input.wav output.wav\n");
        return 1;
    }

    // Open input file for reading
    char *infile = argv[1];
    FILE *inptr = fopen(infile, "rb");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 1;
    }

    // Read header
    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, inptr);

    if (check_format(header) == 0)
    {
        printf("Not a Wave File\n");
        return 1;
    }

    if (header.audioFormat != 1)
    {
        printf("Not a Wave File\n");
        return 1;
    }

    // Open output file for writing
    char *outfile = argv[2];
    FILE *outptr = fopen(outfile, "wb");
    if (outptr == NULL)
    {
        printf("Could not open %s.\n", outfile);
        return 1;
    }

    // Write header to file
    fwrite(&header, sizeof(WAVHEADER), 1, outptr);

    // Use get_block_size to calculate size of block
    int size = get_block_size(header);

    // Seek to the end of the file
    fseek(inptr, 0, SEEK_END);
    long file_size = ftell(inptr);

    // Set the file pointer to the end of the audio data
    fseek(inptr, sizeof(WAVHEADER), SEEK_SET);

    // Create a buffer for audio data
    BYTE *buffer = malloc(size);

    // Read and write the audio data chunk by chunk, in reverse order
    for (long i = file_size - size; i >= sizeof(WAVHEADER); i -= size)
    {
        // Seek to the current position
        fseek(inptr, i, SEEK_SET);

        // Read a chunk of audio data
        fread(buffer, 1, size, inptr);

        // Write the chunk to the output file
        fwrite(buffer, 1, size, outptr);
    }

    // Free the buffer
    free(buffer);

    // Close files
    fclose(outptr);
    fclose(inptr);

    return 0;
}

int check_format(WAVHEADER header)
{
    if (header.format[0] == 'W' && header.format[1] == 'A' && header.format[2] == 'V' && header.format[3] == 'E')
    {
        return 1;
    }
    return 0;
}

int get_block_size(WAVHEADER header)
{
    int size = header.numChannels * header.bitsPerSample / 8;
    return size;
}
