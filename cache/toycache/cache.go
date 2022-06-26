package toycache

import(
	"sync"
	"lru"
)

type Cache struct {
	mtx         sync.Mutex
	lru        *lru.LruCache
	cacheBytes int64
}

func (c *Cache) Add(key string, value ByteView){
	c.mtx.Lock()
	defer c.mtx.Unlock()

	//延迟初始化
	if c.lru == nil {
		c.lru = lru.NewLru(c.cacheBytes, nil)
	}

	c.lru.Add(key, value)
}

func (c *Cache) Get(key string) (value ByteView, ok bool){
	c.mtx.Lock()
	defer c.mtx.Unlock()

	if c.lru == nil {
		return
	} else {
		v, ok := c.lru.Get(key)
		if ok {
			return v.(ByteView), ok
		} else {
			return ByteView{}, false
		}
	}
}