module toycache

go 1.13

require lru v0.0.0

replace lru => ./lru

require consistenthash v0.0.0

replace consistenthash => ./consistenthash
