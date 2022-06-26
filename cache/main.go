package main

import(
	"fmt"
	"log"
	"net/http"
	"toycache"
)

var virtualDb = map[string]string{
	"Tom":  "630",
	"Jack": "589",
	"Sam":  "567",
}

func main() {
	toycache.NewChannel("scores", 2<<10, toycache.GetterFunc(
		func(key string) ([]byte, error) {
			log.Println("[SlowDB] search key", key)
			if v, ok := virtualDb[key]; ok {
				return []byte(v), nil
			}
			return nil, fmt.Errorf("%s not exist", key)
		}))

	addr := "localhost:10010"
	peers := toycache.NewHTTPPool(addr)
	log.Println("toycache is running at", addr)
	log.Fatal(http.ListenAndServe(addr, peers))
}