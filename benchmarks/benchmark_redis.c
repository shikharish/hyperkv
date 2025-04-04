#include "benchmark.h"

// #define HAVE_HIREDIS
#ifdef HAVE_HIREDIS
#include <hiredis/hiredis.h>

// Helper function to get current time in milliseconds
extern double get_time_ms();

// Helper function to connect to Redis
static redisContext *redis_connect(const char *host, int port) {
	redisContext *ctx = redisConnect(host, port);
	if (ctx == NULL || ctx->err) {
		if (ctx) {
			fprintf(stderr, "Redis connection error: %s\n", ctx->errstr);
			redisFree(ctx);
		} else {
			fprintf(stderr, "Redis connection error: can't allocate redis context\n");
		}
		exit(1);
	}
	return ctx;
}

// Benchmark string operations (SET/GET) with Redis
static BenchmarkResult benchmark_string_ops_redis(redisContext *ctx, int num_ops, int key_size,
												  int value_size) {
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
		redisReply *reply = redisCommand(ctx, "SET %s %s", keys[i], values[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis SET: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Benchmark GET operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		redisReply *reply = redisCommand(ctx, "GET %s", keys[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis GET: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Clean up
	for (int i = 0; i < num_ops; i++) {
		free(keys[i]);
		free(values[i]);
	}
	free(keys);
	free(values);

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

// Benchmark hash operations (HSET/HGET) with Redis
static BenchmarkResult benchmark_hash_ops_redis(redisContext *ctx, int num_ops, int key_size,
												int field_size, int value_size) {
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
		redisReply *reply =
			redisCommand(ctx, "HSET %s %s %s", hash_keys[hash_idx], fields[i], values[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis HSET: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Benchmark HGET operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int hash_idx = i / fields_per_hash;
		redisReply *reply = redisCommand(ctx, "HGET %s %s", hash_keys[hash_idx], fields[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis HGET: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

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

// Benchmark list operations (LPUSH/LPOP) with Redis
static BenchmarkResult benchmark_list_ops_redis(redisContext *ctx, int num_ops, int key_size,
												int value_size) {
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
		redisReply *reply = redisCommand(ctx, "LPUSH %s %s", list_keys[list_idx], values[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis LPUSH: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Benchmark LPOP operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int list_idx = i / items_per_list;
		redisReply *reply = redisCommand(ctx, "LPOP %s", list_keys[list_idx]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis LPOP: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Clean up
	for (int i = 0; i < num_lists; i++) {
		free(list_keys[i]);
	}
	for (int i = 0; i < num_ops; i++) {
		free(values[i]);
	}
	free(list_keys);
	free(values);

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

// Benchmark set operations (SADD/SISMEMBER) with Redis
static BenchmarkResult benchmark_set_ops_redis(redisContext *ctx, int num_ops, int key_size,
											   int value_size) {
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
		redisReply *reply = redisCommand(ctx, "SADD %s %s", set_keys[set_idx], values[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis SADD: %.2f ms (%.2f ops/sec)\n", operation_time,
		   num_ops / (operation_time / 1000.0));

	// Benchmark SISMEMBER operations
	start_time = get_time_ms();
	for (int i = 0; i < num_ops; i++) {
		int set_idx = i / items_per_set;
		redisReply *reply = redisCommand(ctx, "SISMEMBER %s %s", set_keys[set_idx], values[i]);
		freeReplyObject(reply);
	}
	end_time = get_time_ms();
	operation_time = end_time - start_time;
	total_time += operation_time;
	printf("Redis SISMEMBER: %.2f ms (%.2f ops/sec)\n", operation_time,
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
BenchmarkResult run_redis_benchmark(BenchmarkConfig config) {
	if (!config.is_local) {
		printf("Running Redis benchmark with %d operations...\n", config.num_operations);

		// Connect to Redis
		redisContext *ctx = redis_connect(config.redis_host, config.redis_port);

		// Flush Redis database before benchmark
		redisReply *reply = redisCommand(ctx, "FLUSHALL");
		freeReplyObject(reply);

		BenchmarkResult result;

		switch (config.type) {
		case BM_STRING:
			result = benchmark_string_ops_redis(ctx, config.num_operations, config.key_size,
												config.value_size);
			break;
		case BM_HASH:
			result = benchmark_hash_ops_redis(ctx, config.num_operations, config.key_size,
											  config.key_size, config.value_size);
			break;
		case BM_LIST:
			result = benchmark_list_ops_redis(ctx, config.num_operations, config.key_size,
											  config.value_size);
			break;
		case BM_SET:
			result = benchmark_set_ops_redis(ctx, config.num_operations, config.key_size,
											 config.value_size);
			break;
		case BM_MIXED:
			// For mixed benchmarks, we'll split operations between different types
			printf("Running mixed Redis benchmark (25%% each type)...\n");
			int ops_per_type = config.num_operations / 4;

			BenchmarkResult string_result =
				benchmark_string_ops_redis(ctx, ops_per_type, config.key_size, config.value_size);
			BenchmarkResult hash_result = benchmark_hash_ops_redis(
				ctx, ops_per_type, config.key_size, config.key_size, config.value_size);
			BenchmarkResult list_result =
				benchmark_list_ops_redis(ctx, ops_per_type, config.key_size, config.value_size);
			BenchmarkResult set_result =
				benchmark_set_ops_redis(ctx, ops_per_type, config.key_size, config.value_size);

			// Combine results
			result = (BenchmarkResult){
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
			break;
		default:
			fprintf(stderr, "Unknown benchmark type\n");
			exit(1);
		}

		// Close Redis connection
		redisFree(ctx);

		return result;
	} else {
		fprintf(stderr, "Configuration error: is_local must be false to use run_redis_benchmark\n");
		exit(1);
	}
}

#else // !HAVE_HIREDIS

// Stub implementation when hiredis is not available
BenchmarkResult run_redis_benchmark(BenchmarkConfig config) {
	fprintf(stderr,
			"Redis benchmarking is not available. Compile with HAVE_HIREDIS defined to enable.\n");

	// Return empty result
	BenchmarkResult result = {.total_time_ms = 0,
							  .ops_per_second = 0,
							  .avg_latency_ms = 0,
							  .type = config.type,
							  .num_operations = config.num_operations,
							  .key_size = config.key_size,
							  .value_size = config.value_size};

	return result;
}

#endif // HAVE_HIREDIS