#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../src/common.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// Benchmark types
typedef enum { BM_STRING, BM_HASH, BM_LIST, BM_SET, BM_MIXED } BenchmarkType;

// Benchmark config
typedef struct {
	int num_operations;		// Number of operations to perform
	int key_size;			// Size of keys in bytes
	int value_size;			// Size of values in bytes
	BenchmarkType type;		// Type of benchmark to run
	bool is_local;			// Whether to benchmark the local implementation or Redis
	const char *redis_host; // Redis server hostname/IP
	int redis_port;			// Redis server port
} BenchmarkConfig;

// Benchmark result
typedef struct {
	double total_time_ms;  // Total time in milliseconds
	double ops_per_second; // Operations per second
	double avg_latency_ms; // Average latency in milliseconds
	BenchmarkType type;	   // The type of benchmark run
	int num_operations;	   // Number of operations performed
	int key_size;		   // Size of keys used
	int value_size;		   // Size of values used
} BenchmarkResult;

// Function to generate random string
char *random_string(int length);

// Function to run a benchmark
BenchmarkResult run_local_benchmark(BenchmarkConfig config);
BenchmarkResult run_redis_benchmark(BenchmarkConfig config);

// Functions to print benchmark results
void print_benchmark_result(BenchmarkResult result);
void print_benchmark_comparison(BenchmarkResult hyperkv, BenchmarkResult redis);

#endif // BENCHMARK_H