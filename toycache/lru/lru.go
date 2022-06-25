package lru

import "container/list"

type LruCache struct{
	//双向链表
	lruList		*list.List
	lruMap		map[string]*list.Element
	ActionCb		func(key string, value Value)

	maxBytes	int64
	curBytes	int64
}

//双向链表节点的数据类型
type entry struct {
	key   string
	value Value
}

//map值的数据类型
type Value interface{
	Len()	int
}

func NewLru(maxBytes int64, ActionCb func(string, Value)) *LruCache {
	return &LruCache{
		maxBytes:  	maxBytes,
		lruList:    list.New(),
		lruMap:    	make(map[string]*list.Element),
		ActionCb: 	ActionCb,
	}
}

//每次查找一个key, 就要返回对应的value并将该key移动到链表尾部
func (cache *LruCache)Get(key string) (value Value, ok bool){
	if element, ok := cache.lruMap[key]; ok {
		cache.lruList.MoveToFront(element)
		kv := element.Value.(*entry)
		return kv.value, true
	}
	return
}

func (cache *LruCache) Update() {
	element := cache.lruList.Back()
	if element != nil {
		cache.lruList.Remove(element)
		kv := element.Value.(*entry)
		delete(cache.lruMap, kv.key)
		cache.curBytes -= int64(len(kv.key)) + int64(kv.value.Len())
		if cache.ActionCb != nil {
			cache.ActionCb(kv.key, kv.value)
		}
	}
}

func (cache *LruCache) Add(key string, value Value) {
	if element, ok := cache.lruMap[key]; ok {
		cache.lruList.MoveToFront(element)
		kv := element.Value.(*entry)
		cache.curBytes += int64(value.Len()) - int64(kv.value.Len())
		kv.value = value
	} else {
		element := cache.lruList.PushFront(&entry{key, value})
		cache.lruMap[key] = element
		cache.curBytes += int64(len(key)) + int64(value.Len())
	}
	for cache.maxBytes != 0 && cache.maxBytes < cache.curBytes {
		cache.Update()
	}
}

func (cache *LruCache) Len() int {
	return cache.lruList.Len()
}