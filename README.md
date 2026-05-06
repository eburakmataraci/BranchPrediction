# Optimization via Branchless Coding in RLE Compression

This project was developed to fix the hardware bottlenecks caused by the "if-else" conditions we usually see in data compression algorithms like Run-Length Encoding (RLE). Instead of using standard conditional branches, we used bitwise operations to stop the processor's pipeline from flushing, which gave our algorithm a massive performance boost. 

![Uygulama Ekran Görüntüsü](images/resim1.jpeg)
![Uygulama Ekran Görüntüsü](images/resim2.jpeg)


## Problem and Solution Approach
* Standard algorithms force the CPU to guess blindly because of complex data patterns.
* When it guesses wrong (misprediction), a lot of clock cycles are wasted.
* We wrote this in C for the x86-64 architecture.
* Instead of if-else blocks, we used mathematical and bitwise operators (`&`, `|`, `~`).
* By doing unconditional memory writes, we completely removed the CPU's need to predict, which drastically cut down latency.

## Performance Results
We tested this on an AMD Ryzen 6800HS processor with a 524 MB dataset using AMD uProf. Here’s what we got:
* **Execution Time:** The baseline took 15.641 seconds, but our branchless version finished in just 6.924 seconds (a 55.7% drop).
* **Throughput:** Jumped from 31.97 MB/s to 72.21 MB/s. That’s a 125.8% increase (about 2.25x faster).
* **Misprediction Rate:** Went from 20-35% down to exactly 0%.
* **CPI (Cycles Per Instruction):** Dropped from ~13.2 to ~2.1, meaning it runs way more efficiently on the hardware.

> **Note:** Even though the bitwise approach makes the compiler generate more assembly instructions, the overall performance is way better because pipeline stalls are completely gone.

## How to Run
You can compile and run both versions of the code. The execution format is: `<program_name> <input_file> <output_file>`

**For the Baseline RLE:**
```bash
gcc baseline_rle.c -o baseline_rle
./baseline_rle input.bin output.bin
```

**For the Branchless RLE:**
```bash
gcc branchless_rle.c -o branchless_rle
./branchless_rle input.bin output.bin
```

## Developers
* Ertuğrul Burak MATARACI
* İlker AYDIN
