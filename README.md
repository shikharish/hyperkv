# HyperKV

HyperKV is a high-performance, in-memory key-value store written in pure C. It features both server and client components with support for various data structures including strings, hashes, lists, and sets.


## Built with

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)

## Build Instructions

Instructions to build the project from source.

```bash
# clone this repository
git clone https://github.com/shikharish/hyperkv.git

# cd into the repository
cd hyperkv

# build the redis server (hyperkv)
make

# build the redis client (hyperkv-cli)
make client

# run the unit tests
make test

# run benchmarks
make benchmark
```

## Usage

`hyperkv` is the server's executable, while `hyperkv-cli` is the client's. You
can run them separately or use the utility script `connect.sh` to run both at
once (useful when developing).

```bash
# run the utility script
chmod +x connect.sh && ./connect.sh

# run the server
./hyperkv

# run the client
./hyperkv-cli
```

## Commands supported

```
cmds list:
---
str cmds:        hash cmds:       list cmds:      set cmds:
- [x] set        - [x] hset       - [x] lpush     - [x] sadd
- [x] get        - [x] hget       - [x] rpush     - [x] srem
- [x] mset       - [x] hdel       - [x] lpop      - [x] smembers
- [x] mget       - [x] hgetall    - [x] rpop      - [x] sismember
- [x] incr       - [x] hexists    - [x] llen      - [ ] scard
- [x] decr       - [x] hkeys      - [x] lindex    - [x] smismember
- [x] incrby     - [x] hvals      - [x] lpos      - [ ] sdiff
- [x] decrby     - [x] hlen       - [x] lset      - [ ] sinter
- [x] strlen     - [ ] hincrby    - [x] lrem      - [ ] sunion
- [ ] append     - [x] hmget      - [x] lrange    - [ ] sdiffstore
- [ ] setrange   - [ ] hstrlen    - [ ] lpushx    - [ ] sinterstore
- [ ] getrange   - [ ] hsetnx     - [ ] rpushx    - [ ] sunionstore
- [ ] setnx      - [ ]            - [ ] ltrim     - [ ]
- [ ] msetnx                      - [ ]
- [ ]


keys cmds:      etc:
- [x] del       - [ ] ping
- [x] exists    - [x] quit
- [x] type      - [x] shutdown
- [ ] rename    - [ ]
- [ ]
```

## Performance Benchmarks

HyperKV includes a benchmarking system to measure performance and compare with Redis. The benchmarks test various operations across different data types:

- String operations (SET/GET)
- Hash operations (HSET/HGET)
- List operations (LPUSH/LPOP)
- Set operations (SADD/SISMEMBER)

To run the benchmarks:

```bash
# Basic benchmarks (HyperKV only)
make benchmark

# With Redis comparison (requires Redis server and hiredis library)
make benchmark_redis

# Run with custom parameters
./benchmark --ops 100000 --type string --key-size 20 --value-size 200
```

See the `benchmarks/README.md` file for detailed information about the benchmarking system.

## License

Distributed under the MIT License. See [LICENSE](/LICENSE) for more information.
