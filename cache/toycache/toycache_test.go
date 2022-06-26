package toycache

import(
	"fmt"
	"log"
	"reflect"
	"testing"
)

var virtualDb = map[string]string {
	"xiaozhang"	:  	"jiaotong",
	"xiaoliu"	: 	"zhexue",
	"hetueyuan"	:  	"tongji",
} 

func TestGetter(t *testing.T) {
	var f Getter = GetterFunc(func(key string) ([]byte, error) {
		return []byte(key), nil
	})

	expect := []byte("key")
	if v, _ := f.Get("key"); !reflect.DeepEqual(v, expect) {
		t.Fatal("callback failed")
	}
}

func TestGet(t *testing.T) {
	loadCounts := make(map[string]int, len(virtualDb))
	toycache := NewChannel("scores", 2<<10, GetterFunc(
		func(key string) ([]byte, error) {
			log.Println("[SlowDB] search key", key)
			if v, ok := virtualDb[key]; ok {
				if _, ok := loadCounts[key]; !ok {
					loadCounts[key] = 0
				}
				loadCounts[key] += 1
				return []byte(v), nil
			}
			return nil, fmt.Errorf("%s not exist", key)
		}))

	for k, v := range virtualDb {
		if view, err := toycache.Get(k); err != nil || view.String() != v {
			t.Fatal("failed to get value of Tom")
		} // load from callback function
		if _, err := toycache.Get(k); err != nil || loadCounts[k] > 1 {
			t.Fatalf("cache %s miss", k)
		} // cache hit
	}

	if view, err := toycache.Get("unknown"); err == nil {
		t.Fatalf("the value of unknow should be empty, but %s got", view)
	}
	if view, err := toycache.Get("xiaozhang"); err != nil {
		t.Fatalf("the value of xiaozhang should mot be empty, but %s got", view)
	} else {
		fmt.Printf("value of %s is %s\n","xiaozhang", view)
	}
	if view, err := toycache.Get("xiaoliu"); err != nil {
		t.Fatalf("the value of xiaoliu should mot be empty, but %s got", view)
	} else {
		fmt.Printf("value of %s is %s\n","xiaoliu", view)
	}
}