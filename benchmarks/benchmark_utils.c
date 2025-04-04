#include "benchmark.h"

// Generate a random string of given length
char *random_string(int length) {
	static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char *str = malloc(length + 1);
	if (str) {
		for (int i = 0; i < length; i++) {
			int index = rand() % (sizeof(charset) - 1);
			str[i] = charset[index];
		}
		str[length] = '\0';
	}
	return str;
}

// Get current time in milliseconds
double get_time_ms() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
}

// Print benchmark result
void print_benchmark_result(BenchmarkResult result) {
	const char *type_str = "";
	switch (result.type) {
	case BM_STRING:
		type_str = "String";
		break;
	case BM_HASH:
		type_str = "Hash";
		break;
	case BM_LIST:
		type_str = "List";
		break;
	case BM_SET:
		type_str = "Set";
		break;
	case BM_MIXED:
		type_str = "Mixed";
		break;
	}

	printf("===== %s Benchmark Results =====\n", type_str);
	printf("Operations: %d (key size: %d, value size: %d)\n", result.num_operations,
		   result.key_size, result.value_size);
	printf("Total time: %.2f ms\n", result.total_time_ms);
	printf("Throughput: %.2f ops/sec\n", result.ops_per_second);
	printf("Avg latency: %.3f ms\n", result.avg_latency_ms);
	printf("==============================\n\n");
}

// Print comparison between HyperKV and Redis
void print_benchmark_comparison(BenchmarkResult hyperkv, BenchmarkResult redis) {
	const char *type_str = "";
	switch (hyperkv.type) {
	case BM_STRING:
		type_str = "String";
		break;
	case BM_HASH:
		type_str = "Hash";
		break;
	case BM_LIST:
		type_str = "List";
		break;
	case BM_SET:
		type_str = "Set";
		break;
	case BM_MIXED:
		type_str = "Mixed";
		break;
	}

	double throughput_ratio = hyperkv.ops_per_second / redis.ops_per_second;
	double latency_ratio = redis.avg_latency_ms / hyperkv.avg_latency_ms;

	printf("===== %s Benchmark Comparison =====\n", type_str);
	printf("Operations: %d (key size: %d, value size: %d)\n", hyperkv.num_operations,
		   hyperkv.key_size, hyperkv.value_size);
	printf("                  HyperKV      Redis      Ratio\n");
	printf("Throughput:      %.2f      %.2f      %.2fx\n", hyperkv.ops_per_second,
		   redis.ops_per_second, throughput_ratio);
	printf("Avg latency:     %.3f ms    %.3f ms    %.2fx\n", hyperkv.avg_latency_ms,
		   redis.avg_latency_ms, latency_ratio);
	printf("====================================\n\n");
}