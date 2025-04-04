#include "../src/common.h"
#include "benchmark.h"

// Helper function to get current time in milliseconds
extern double get_time_ms();

// Benchmark string operations (SET/GET)
static BenchmarkResult benchmark_string_ops(int num_ops, int key_size, int value_size) {
	HashTable *ht = htable_init(num_ops);
	double start_time, end_time, operation_time;
	double total_time = 0;
	char **keys = malloc(num_ops * sizeof(char *));
	char **values = malloc(num_ops * sizeof(char *));

	// Generate random keys and values
	for (int i = 0; i < num_ops; i++) {
		keys[i] = random_string(key_size);
		values[i] = random_string(value_size);
	}

	// Benchmark SET operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		htable_set(ht, keys[i], values[i]);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("SET: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Benchmark GET operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		htable_get(ht, keys[i]);
		// Don't free result, it's a pointer to the hash table's internal memory
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("GET: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Clean up
	for (int i = 0; i < num_ops; i++) {
		free(keys[i]);
		free(values[i]);
	}
	free(keys);
	free(values);
	htable_free(ht);

	// Calculate results
	BenchmarkResult result = {.total_time_ms = total_time,
							  .ops_per_second = (num_ops * 2) / (total_time / 1000.0), // SET + GET
							  .avg_latency_ms = total_time / (num_ops * 2),
							  .type = BM_STRING,
							  .num_operations = num_ops,
							  .key_size = key_size,
							  .value_size = value_size};

	return result;
}

// Benchmark hash operations (HSET/HGET)
static BenchmarkResult benchmark_hash_ops(int num_ops, int key_size, int field_size,
										  int value_size) {
	HashTable *ht = htable_init(num_ops);
	double start_time, end_time, operation_time;
	double total_time = 0;

	// We'll use fixed number of hash keys but multiple fields per hash
	int num_hash_keys = num_ops / 10;
	if (num_hash_keys < 1)
		num_hash_keys = 1;

	int fields_per_hash = num_ops / num_hash_keys;

	char **hash_keys = malloc(num_hash_keys * sizeof(char *));
	char **fields = malloc(num_ops * sizeof(char *));
	char **values = malloc(num_ops * sizeof(char *));

	// Generate random keys, fields and values
	for (int i = 0; i < num_hash_keys; i++) {
		hash_keys[i] = random_string(key_size);
	}

	for (int i = 0; i < num_ops; i++) {
		fields[i] = random_string(field_size);
		values[i] = random_string(value_size);
	}

	// Benchmark HSET operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int hash_idx = i / fields_per_hash;
		htable_hset(ht, hash_keys[hash_idx], fields[i], values[i]);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("HSET: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Benchmark HGET operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int hash_idx = i / fields_per_hash;
		htable_hget(ht, hash_keys[hash_idx], fields[i]);
		// Don't free result, it's a pointer to the hash table's internal memory
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("HGET: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Clean up
	for (int i = 0; i < num_hash_keys; i++) {
		free(hash_keys[i]);
	}
	for (int i = 0; i < num_ops; i++) {
		free(fields[i]);
		free(values[i]);
	}
	free(hash_keys);
	free(fields);
	free(values);
	htable_free(ht);

	// Calculate results
	BenchmarkResult result = {.total_time_ms = total_time,
							  .ops_per_second =
								  (num_ops * 2) / (total_time / 1000.0), // HSET + HGET
							  .avg_latency_ms = total_time / (num_ops * 2),
							  .type = BM_HASH,
							  .num_operations = num_ops,
							  .key_size = key_size,
							  .value_size = value_size};

	return result;
}

// Benchmark list operations (LPUSH/LPOP, RPUSH/RPOP)
static BenchmarkResult benchmark_list_ops(int num_ops, int key_size, int value_size) {
	HashTable *ht = htable_init(num_ops);
	double start_time, end_time, operation_time;
	double total_time = 0;

	int num_lists = num_ops / 100;
	if (num_lists < 1)
		num_lists = 1;

	int items_per_list = num_ops / num_lists;

	char **list_keys = malloc(num_lists * sizeof(char *));
	char **values = malloc(num_ops * sizeof(char *));

	// Generate random keys and values
	for (int i = 0; i < num_lists; i++) {
		list_keys[i] = random_string(key_size);
	}

	for (int i = 0; i < num_ops; i++) {
		values[i] = random_string(value_size);
	}

	// Benchmark LPUSH operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int list_idx = i / items_per_list;
		htable_push(ht, list_keys[list_idx], values[i], LEFT);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("LPUSH: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Benchmark LPOP operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int list_idx = i / items_per_list;
		char *result = htable_pop(ht, list_keys[list_idx], LEFT);
		// Note: For LPOP, the result is actually a new string that needs to be freed
		// according to the implementation of htable_pop
		free(result);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("LPOP: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Clean up
	for (int i = 0; i < num_lists; i++) {
		free(list_keys[i]);
	}
	for (int i = 0; i < num_ops; i++) {
		free(values[i]);
	}
	free(list_keys);
	free(values);
	htable_free(ht);

	// Calculate results
	BenchmarkResult result = {.total_time_ms = total_time,
							  .ops_per_second =
								  (num_ops * 2) / (total_time / 1000.0), // LPUSH + LPOP
							  .avg_latency_ms = total_time / (num_ops * 2),
							  .type = BM_LIST,
							  .num_operations = num_ops,
							  .key_size = key_size,
							  .value_size = value_size};

	return result;
}

// Benchmark set operations (SADD/SISMEMBER)
static BenchmarkResult benchmark_set_ops(int num_ops, int key_size, int value_size) {
	HashTable *ht = htable_init(num_ops);
	double start_time, end_time, operation_time;
	double total_time = 0;

	int num_sets = num_ops / 100;
	if (num_sets < 1)
		num_sets = 1;

	int items_per_set = num_ops / num_sets;

	char **set_keys = malloc(num_sets * sizeof(char *));
	char **values = malloc(num_ops * sizeof(char *));

	// Generate random keys and values
	for (int i = 0; i < num_sets; i++) {
		set_keys[i] = random_string(key_size);
	}

	for (int i = 0; i < num_ops; i++) {
		values[i] = random_string(value_size);
	}

	// Benchmark SADD operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int set_idx = i / items_per_set;
		htable_sadd(ht, set_keys[set_idx], values[i]);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("SADD: %.2f ms (%.2f ops/sec)\n", operation_time, num_ops / (operation_time / 1000.0));

	// Benchmark SISMEMBER operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int set_idx = i / items_per_set;
		htable_sismember(ht, set_keys[set_idx], values[i]);
		// No result to free, it's a boolean return
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("SISMEMBER: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Clean up
	for (int i = 0; i < num_sets; i++) {
		free(set_keys[i]);
	}
	for (int i = 0; i < num_ops; i++) {
		free(values[i]);
	}
	free(set_keys);
	free(values);
	htable_free(ht);

	// Calculate results
	BenchmarkResult result = {.total_time_ms = total_time,
							  .ops_per_second =
								  (num_ops * 2) / (total_time / 1000.0), // SADD + SISMEMBER
							  .avg_latency_ms = total_time / (num_ops * 2),
							  .type = BM_SET,
							  .num_operations = num_ops,
							  .key_size = key_size,
							  .value_size = value_size};

	return result;
}

// Run a benchmark based on configuration
BenchmarkResult run_local_benchmark(BenchmarkConfig config) {
	printf("Running local HyperKV benchmark with %d operations...\n", config.num_operations);

	switch (config.type) {
	case BM_STRING:
		return benchmark_string_ops(config.num_operations, config.key_size, config.value_size);
	case BM_HASH:
		return benchmark_hash_ops(config.num_operations, config.key_size, config.key_size,
								  config.value_size);
	case BM_LIST:
		return benchmark_list_ops(config.num_operations, config.key_size, config.value_size);
	case BM_SET:
		return benchmark_set_ops(config.num_operations, config.key_size, config.value_size);
	case BM_MIXED:
		// For mixed benchmarks, we'll split operations between different types
		printf("Running mixed benchmark (25%% each type)...\n");
		int ops_per_type = config.num_operations / 4;

		BenchmarkResult string_result =
			benchmark_string_ops(ops_per_type, config.key_size, config.value_size);
		BenchmarkResult hash_result =
			benchmark_hash_ops(ops_per_type, config.key_size, config.key_size, config.value_size);
		BenchmarkResult list_result =
			benchmark_list_ops(ops_per_type, config.key_size, config.value_size);
		BenchmarkResult set_result =
			benchmark_set_ops(ops_per_type, config.key_size, config.value_size);

		// Combine results
		BenchmarkResult combined = {
			.total_time_ms = string_result.total_time_ms + hash_result.total_time_ms +
							 list_result.total_time_ms + set_result.total_time_ms,
			.ops_per_second =
				(ops_per_type * 8) / ((string_result.total_time_ms + hash_result.total_time_ms +
									   list_result.total_time_ms + set_result.total_time_ms) /
									  1000.0),
			.avg_latency_ms = (string_result.avg_latency_ms + hash_result.avg_latency_ms +
							   list_result.avg_latency_ms + set_result.avg_latency_ms) /
							  4.0,
			.type = BM_MIXED,
			.num_operations = config.num_operations,
			.key_size = config.key_size,
			.value_size = config.value_size};

		return combined;
	default:
		fprintf(stderr, "Unknown benchmark type\n");
		exit(1);
	}
}