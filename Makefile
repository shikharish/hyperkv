CC=gcc
FLAGS=-g -Wall -lm -DLOG_USE_COLOR
SRC=$(wildcard src/*.c)
SERVER=$(filter-out src/hyperkv-cli.c, $(SRC))
CLIENT=$(filter-out src/hyperkv.c, $(SRC))
TEST=$(filter-out src/hyperkv.c src/hyperkv-cli.c, $(wildcard $(SRC) tests/*.c))
BENCHMARK_SRC=benchmarks/benchmark.c benchmarks/benchmark_utils.c benchmarks/benchmark_local.c
BENCHMARK_REDIS_SRC=$(BENCHMARK_SRC) benchmarks/benchmark_redis.c
HIREDIS_FLAGS=-lhiredis -DHAVE_HIREDIS

all: server client

server: $(SERVER)
	$(CC)  $(SERVER) -o hyperkv $(FLAGS)
client: $(CLIENT)
	$(CC)  $(CLIENT) -o hyperkv-cli $(FLAGS)

# Regular test - shows all log output
test: $(TEST)
	$(CC)  $(TEST) -o test.out $(FLAGS) && ./test.out 

# Test with quiet logs (using command line argument)
test-quiet: $(TEST)
	$(CC)  $(TEST) -o test.out $(FLAGS) && ./test.out --test

# Test with quiet logs (using environment variable)
test-env-quiet: $(TEST)
	$(CC)  $(TEST) -o test.out $(FLAGS) && HYPERKV_LOG_LEVEL=TEST ./test.out

# Test with debug logs (using environment variable)
test-env-debug: $(TEST)
	$(CC)  $(TEST) -o test.out $(FLAGS) && HYPERKV_LOG_LEVEL=DEBUG ./test.out

# Standard benchmark without Redis comparison (with logging disabled)
benchmark: $(BENCHMARK_SRC) server
	$(CC) $(BENCHMARK_SRC) $(filter-out src/hyperkv.c src/hyperkv-cli.c, $(SRC)) -o hyperkv_benchmark $(FLAGS)

# Benchmark with Redis comparison (with logging disabled)
benchmark_redis: $(BENCHMARK_REDIS_SRC) server
	$(CC) $(BENCHMARK_REDIS_SRC) $(filter-out src/hyperkv.c src/hyperkv-cli.c, $(SRC)) -o hyperkv_benchmark_redis $(FLAGS) $(HIREDIS_FLAGS) && HYPERKV_LOG_LEVEL=TEST ./hyperkv_benchmark_redis

# Explicit quiet benchmark targets for when you want to be absolutely sure no logs appear
benchmark-quiet: $(BENCHMARK_SRC) server
	$(CC) $(BENCHMARK_SRC) $(filter-out src/hyperkv.c src/hyperkv-cli.c, $(SRC)) -o hyperkv_benchmark $(FLAGS) && HYPERKV_LOG_QUIET=true HYPERKV_LOG_LEVEL=TEST ./hyperkv_benchmark

benchmark_redis-quiet: $(BENCHMARK_REDIS_SRC) server
	$(CC) $(BENCHMARK_REDIS_SRC) $(filter-out src/hyperkv.c src/hyperkv-cli.c, $(SRC)) -o hyperkv_benchmark_redis $(FLAGS) $(HIREDIS_FLAGS) && HYPERKV_LOG_QUIET=true HYPERKV_LOG_LEVEL=TEST ./hyperkv_benchmark_redis

clean:
	rm -rf hyperkv* *.out
