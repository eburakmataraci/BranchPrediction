#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// Bit-düzeyinde optimize edilmiş, dallanmasız (branchless) RLE algoritması.
void rle_compress_branchless(const uint8_t *input, size_t length, FILE *output_file)
{
    if (length == 0)
        return;

    // Dallanmasız dosya I/O (Girdi/Çıktı) işlemi yapılamayacağı için,
    // tüm veriyi geçici bir bellekte (buffer) toplayıp en son tek seferde diske yazacağız.
    // Bu işlem I/O darboğazını ortadan kaldırıp Throughput'u (MB/s) devasa oranda artırır.
    uint8_t *out_buffer = (uint8_t *)malloc(length * 2);
    if (!out_buffer)
    {
        printf("Bellek tahsis hatası!\n");
        return;
    }

    uint8_t curr = input[0];
    uint8_t count = 1;
    size_t write_idx = 0;

    for (size_t i = 1; i < length; i++)
    {
        uint8_t next = input[i];

        // 1. FLUSH (Yazma) DURUMUNU TESPİT ETME
        uint8_t is_diff = (curr != next);

        uint8_t is_full = (count == 255);

        uint8_t flush = is_diff | is_full;

        // 2. ŞARTSIZ BELLEK YAZIMI (Unconditional Memory Write)
        out_buffer[write_idx * 2] = count;
        out_buffer[write_idx * 2 + 1] = curr;

        write_idx += flush;

        // 3. BİT DÜZEYİNDE MATEMATİKSEL DURUM GÜNCELLEMESİ
        uint8_t mask = flush * 0xFF;

        count++;
        count = (count & ~mask) | (1 & mask);

        curr = (curr & ~mask) | (next & mask);
    }

    out_buffer[write_idx * 2] = count;
    out_buffer[write_idx * 2 + 1] = curr;
    write_idx++;

    // Tüm veriyi tek seferde (batch) diske yaz
    fwrite(out_buffer, 1, write_idx * 2, output_file);
    free(out_buffer);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Kullanım: %s <girdi_dosyasi> <cikti_dosyasi>\n", argv[0]);
        return 1;
    }

    FILE *in_file = fopen(argv[1], "rb");
    FILE *out_file = fopen(argv[2], "wb");

    if (!in_file || !out_file)
        return 1;

    fseek(in_file, 0, SEEK_END);
    size_t file_size = ftell(in_file);
    fseek(in_file, 0, SEEK_SET);

    uint8_t *buffer = (uint8_t *)malloc(file_size);
    if (!buffer)
        return 1;

    fread(buffer, 1, file_size, in_file);

    clock_t start_time = clock();

    rle_compress_branchless(buffer, file_size, out_file);

    clock_t end_time = clock();

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    double throughput = (file_size / (1024.0 * 1024.0)) / time_spent;

    printf("Optimized Compression Completed.\n");
    printf("Processed Data: %zu bytes\n", file_size);
    printf("Elapsed Time: %f seconds\n", time_spent);
    printf("Throughput: %.2f MB/s\n", throughput);

    free(buffer);
    fclose(in_file);
    fclose(out_file);

    return 0;
}