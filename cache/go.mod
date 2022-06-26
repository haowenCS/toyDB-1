module main

go 1.13

require toycache v0.0.0

replace toycache => ./toycache

replace lru => ./toycache/lru
