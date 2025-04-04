CC=gcc
FLAGS=-g -Wall -lm
SRC=$(wildcard src/*.c)
SERVER=$(filter-out src/hyperkv-cli.c, $(SRC))
CLIENT=$(filter-out src/hyperkv.c, $(SRC))
TEST=$(filter-out src/hyperkv.c src/hyperkv-cli.c, $(wildcard $(SRC) tests/*.c))

all: server client

server: $(SERVER)
	$(CC)  $(SERVER) -o hyperkv $(FLAGS)
client: $(CLIENT)
	$(CC)  $(CLIENT) -o hyperkv-cli $(FLAGS)
test: $(TEST)
	$(CC)  $(TEST) -o test.out $(FLAGS) && ./test.out 
clean:
	rm hyperkv* *.out
