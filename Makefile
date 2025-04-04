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
test: $(TEST)
	$(CC)  $(TEST) -o test.out $(FLAGS) && ./test.out 

# Standard benchmark without Redis comparison
benchmark: $(BENCHMARK_SRC) server
	$(CC) $(BENCHMARK_SRC) $(filter-out src/hyperkv.c src/hyperkv-cli.c, $(SRC)) -o hyperkv_benchmark $(FLAGS)

# Benchmark with Redis comparison (requires hiredis library)
benchmark_redis: $(BENCHMARK_REDIS_SRC) server
	$(CC) $(BENCHMARK_REDIS_SRC) $(filter-out src/hyperkv.c src/hyperkv-cli.c, $(SRC)) -o hyperkv_benchmark_redis $(FLAGS) $(HIREDIS_FLAGS)

clean:
	rm -rf hyperkv* *.out
