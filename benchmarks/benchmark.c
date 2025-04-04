#include "benchmark.h"
#include "../src/log.h"
#include "../src/log_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
	// Initialize logging in test mode (no logs) for benchmarks
	// Check environment variables first
	char *env_log_level = getenv("HYPERKV_LOG_LEVEL");
	if (env_log_level != NULL) {
		log_init_from_env();
	} else {
		// Default to test mode (no logs) for benchmarks
		log_init_testing();
	}
	
	// Seed random number generator
	srand(time(NULL));

	// Default configuration
	BenchmarkConfig config = {.num_operations = 10000,
							  .key_size = 10,
							  .value_size = 100,
							  .type = BM_STRING,
							  .is_local = true,
							  .redis_host = "localhost",
							  .redis_port = 6379};

	// Parse command line arguments
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--ops") == 0 && i + 1 < argc) {
			config.num_operations = atoi(argv[i + 1]);
			i++;
		} else if (strcmp(argv[i], "--key-size") == 0 && i + 1 < argc) {
			config.key_size = atoi(argv[i + 1]);
			i++;
		} else if (strcmp(argv[i], "--value-size") == 0 && i + 1 < argc) {
			config.value_size = atoi(argv[i + 1]);
			i++;
		} else if (strcmp(argv[i], "--type") == 0 && i + 1 < argc) {
			if (strcmp(argv[i + 1], "string") == 0) {
				config.type = BM_STRING;
			} else if (strcmp(argv[i + 1], "hash") == 0) {
				config.type = BM_HASH;
			} else if (strcmp(argv[i + 1], "list") == 0) {
				config.type = BM_LIST;
			} else if (strcmp(argv[i + 1], "set") == 0) {
				config.type = BM_SET;
			} else if (strcmp(argv[i + 1], "mixed") == 0) {
				config.type = BM_MIXED;
			} else {
				fprintf(stderr, "Unknown benchmark type: %s\n", argv[i + 1]);
				return 1;
			}
			i++;
		} else if (strcmp(argv[i], "--redis-host") == 0 && i + 1 < argc) {
			config.redis_host = argv[i + 1];
			i++;
		} else if (strcmp(argv[i], "--redis-port") == 0 && i + 1 < argc) {
			config.redis_port = atoi(argv[i + 1]);
			i++;
		} else if (strcmp(argv[i], "--help") == 0) {
			printf("Usage: %s [OPTIONS]\n\n", argv[0]);
			printf("Options:\n");
			printf("  --ops NUMBER          Number of operations to perform (default: 10000)\n");
			printf("  --key-size SIZE       Size of keys in bytes (default: 10)\n");
			printf("  --value-size SIZE     Size of values in bytes (default: 100)\n");
			printf("  --type TYPE           Type of benchmark to run (string, hash, list, set, "
				   "mixed)\n");
			printf("  --redis-host HOST     Redis server hostname/IP (default: localhost)\n");
			printf("  --redis-port PORT     Redis server port (default: 6379)\n");
			printf("  --help                Display this help message\n");
			return 0;
		} else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			return 1;
		}
	}

	// Print benchmark configuration
	printf("=== Benchmark Configuration ===\n");
	printf("Operations: %d\n", config.num_operations);
	printf("Key size: %d bytes\n", config.key_size);
	printf("Value size: %d bytes\n", config.value_size);
	printf("Type: ");
	switch (config.type) {
	case BM_STRING:
		printf("String\n");
		break;
	case BM_HASH:
		printf("Hash\n");
		break;
	case BM_LIST:
		printf("List\n");
		break;
	case BM_SET:
		printf("Set\n");
		break;
	case BM_MIXED:
		printf("Mixed\n");
		break;
	}
	printf("==============================\n\n");

	// Run HyperKV benchmark
	config.is_local = true;
	BenchmarkResult hyperkv_result = run_local_benchmark(config);
	print_benchmark_result(hyperkv_result);

// Check if hiredis is available to run Redis benchmark
#ifdef HAVE_HIREDIS
	// Run Redis benchmark
	printf("Running Redis benchmark. Make sure Redis server is running on %s:%d\n",
		   config.redis_host, config.redis_port);
	config.is_local = false;
	BenchmarkResult redis_result = run_redis_benchmark(config);
	print_benchmark_result(redis_result);

	// Compare results
	print_benchmark_comparison(hyperkv_result, redis_result);
#else
	printf("Redis benchmarking is disabled. Compile with HAVE_HIREDIS defined to enable.\n");
#endif

	return 0;
}