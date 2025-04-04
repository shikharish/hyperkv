# HyperKV Benchmarking System

This directory contains benchmarking tools for HyperKV, allowing performance testing and comparison with Redis.

## Overview

The benchmarking system tests the performance of various operations in HyperKV:

- String operations (SET/GET)
- Hash operations (HSET/HGET)
- List operations (LPUSH/LPOP)
- Set operations (SADD/SISMEMBER)
- Mixed operations (a combination of all types)

## Requirements

- C compiler (GCC recommended)
- Redis server (for comparison benchmarks)
- Hiredis library (for Redis client connection)

## Installing Hiredis

To enable Redis benchmarking for comparison, you'll need to install the Hiredis library:

```bash
# Ubuntu/Debian
sudo apt-get install libhiredis-dev

# macOS (with Homebrew)
brew install hiredis

# Manual installation
git clone https://github.com/redis/hiredis.git
cd hiredis
make
sudo make install
```

## Usage

```bash
# Run benchmarks with default settings
make benchmark

# Run specific benchmark with custom parameters
./benchmark --ops 100000 --key-size 20 --value-size 200 --type string
```

### Command Line Options

- `--ops NUMBER`: Number of operations to perform (default: 10000)
- `--key-size SIZE`: Size of keys in bytes (default: 10)
- `--value-size SIZE`: Size of values in bytes (default: 100)
- `--type TYPE`: Type of benchmark to run (string, hash, list, set, mixed)
- `--redis-host HOST`: Redis server hostname/IP (default: localhost)
- `--redis-port PORT`: Redis server port (default: 6379)
- `--help`: Display help message

## Interpreting Results

The benchmark outputs the following metrics:

- Total time: Total execution time in milliseconds
- Throughput: Operations per second
- Average latency: Average time per operation in milliseconds

When Redis comparison is enabled, it also shows the performance ratio between HyperKV and Redis.

## Example Output

```
=== Benchmark Configuration ===
Operations: 10000
Key size: 10 bytes
Value size: 100 bytes
Type: String
==============================

Running local HyperKV benchmark with 10000 operations...
SET: 15.24 ms (655997.54 ops/sec)
GET: 10.12 ms (987654.32 ops/sec)
===== String Benchmark Results =====
Operations: 10000 (key size: 10, value size: 100)
Total time: 25.36 ms
Throughput: 787420.94 ops/sec
Avg latency: 0.001 ms
==============================

Running Redis benchmark. Make sure Redis server is running on localhost:6379
Redis SET: 120.45 ms (83022.00 ops/sec)
Redis GET: 95.78 ms (104405.93 ops/sec)
===== String Benchmark Results =====
Operations: 10000 (key size: 10, value size: 100)
Total time: 216.23 ms
Throughput: 92494.57 ops/sec
Avg latency: 0.011 ms
==============================

===== String Benchmark Comparison =====
Operations: 10000 (key size: 10, value size: 100)
                  HyperKV      Redis      Ratio
Throughput:      787420.94      92494.57      8.51x
Avg latency:     0.001 ms    0.011 ms    8.51x
====================================
```
