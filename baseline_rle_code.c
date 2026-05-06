#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// Basic (Naive) Run-Length Encoding Compression Function
void rle_compress(const uint8_t *input, size_t length, FILE *output_file)
{
    if (length == 0)
        return;

    uint8_t current_char = input[0];
    size_t count = 1;

    for (size_t i = 1; i < length; i++)
    {
        // This is the most critical branch point where we would
        // observe mispredictions in profiling tools.
        if (input[i] == current_char)
        {
            count++;

            if (count == 255)
            {
                fwrite(&count, 1, 1, output_file);
                fwrite(&current_char, 1, 1, output_file);
                count = 0;

                if (i + 1 < length)
                {
                    current_char = input[i + 1];
                    i++;
                    count = 1;
                }
            }
        }
        else
        {
            if (count > 0)
            {
                fwrite(&count, 1, 1, output_file);
                fwrite(&current_char, 1, 1, output_file);
            }
            current_char = input[i];
            count = 1;
        }
    }

    if (count > 0)
    {
        fwrite(&count, 1, 1, output_file);
        fwrite(&current_char, 1, 1, output_file);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *in_file = fopen(argv[1], "rb");
    FILE *out_file = fopen(argv[2], "wb");

    if (!in_file || !out_file)
    {
        printf("File opening error! Check input and output paths.\n");
        return 1;
    }

    // Measure input file size and allocate memory
    fseek(in_file, 0, SEEK_END);
    size_t file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    uint8_t *buffer = (uint8_t *)malloc(file_size);
    if (!buffer)
    {
        printf("Memory allocation error (Malloc failed)!\n");
        fclose(in_file);
        fclose(out_file);
        return 1;
    }

    fread(buffer, 1, file_size, in_file);

    // Start time measurement for throughput and hardware metrics
    clock_t start_time = clock();

    rle_compress(buffer, file_size, out_file);

    clock_t end_time = clock();

    // Basic performance calculations
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    double throughput = (file_size / (1024.0 * 1024.0)) / time_spent; // Speed in MB/s

    printf("Compression Completed.\n");
    printf("Processed Data: %zu bytes\n", file_size);
    printf("Elapsed Time: %f seconds\n", time_spent);
    printf("Throughput: %.2f MB/s\n", throughput);

    // Cleanup to prevent memory leaks
    free(buffer);
    fclose(in_file);
    fclose(out_file);

    return 0;
}
