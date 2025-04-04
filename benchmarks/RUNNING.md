# Running HyperKV Benchmarks

This guide explains how to run the HyperKV benchmarks and compare performance with Redis.

## Prerequisites

1. Make sure you have compiled HyperKV successfully
2. For Redis comparison:
   - Install Redis server on your system
   - Install the Hiredis library

## Compiling the Benchmarks

### Basic Benchmarks (HyperKV only)

```bash
make benchmark
```

### Redis Comparison Benchmarks

```bash
make benchmark_redis
```

If you encounter any issues with Redis benchmarking, ensure that:

- Redis server is installed and running (`redis-server`)
- Hiredis library is installed properly
- Your compiler can find the Hiredis headers and libraries

## Running the Benchmarks

### Basic Usage

```bash
# Run default benchmarks (string operations, 10000 ops)
./benchmark

# If you compiled with Redis support:
./benchmark_redis

# Show help
./benchmark --help
```

### Testing Specific Data Types

```bash
# String operations
./benchmark --type string --ops 100000

# Hash operations
./benchmark --type hash --ops 50000

# List operations
./benchmark --type list --ops 50000

# Set operations
./benchmark --type set --ops 50000

# Mixed workload
./benchmark --type mixed --ops 40000
```

### Customizing Benchmark Parameters

```bash
# Change key and value sizes
./benchmark --key-size 20 --value-size 500

# Specify Redis host and port (if using Redis comparison)
./benchmark_redis --redis-host 192.168.1.100 --redis-port 6380
```

## Interpreting Results

The benchmark will output:

1. Configuration details
2. Raw operation times for each operation type
3. Summary statistics for HyperKV
4. Summary statistics for Redis (if enabled)
5. Comparison between HyperKV and Redis (if enabled)

Pay special attention to:

- Throughput (operations per second)
- Average latency (ms per operation)
- Ratio between HyperKV and Redis performance

## Troubleshooting

If you encounter issues:

1. **Memory Errors**: If you get errors about freeing pointers that were not allocated or segmentation faults:

   - This was a known issue in earlier versions where the benchmark code incorrectly tried to free memory returned by htable_get() and similar functions
   - The latest code fixes this issue by not freeing pointers that are managed by the hash table
   - Ensure you're using the latest version of the benchmark code

2. Verify Redis is running: `redis-cli ping` should return `PONG`
3. Check if Hiredis is correctly installed: `ls -l /usr/local/include/hiredis` or similar
4. Try with smaller operation counts first (e.g., `--ops 1000`) to ensure everything works
5. If you get compilation errors with `benchmark_redis`, try the regular `benchmark` target

## Memory Management Notes

The HyperKV functions handle memory differently:

- `htable_get()`, `htable_hget()`: Return pointers to internal memory that should NOT be freed
- `htable_pop()`: Returns newly allocated strings that SHOULD be freed after use
- `htable_type()`: Returns newly allocated strings that should be freed

If you're modifying the benchmark code, be careful about these memory management differences.
