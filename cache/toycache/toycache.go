package toycache

import (
	"fmt"
	"sync"
	"log"
)

// A Getter loads data for a key.
type Getter interface {
	Get(key string) ([]byte, error)
}

// A GetterFunc implements Getter with a function.
type GetterFunc func(key string) ([]byte, error)

//回调
//函数类型实现某一个接口，称为`接口型函数`
func (f GetterFunc) Get(key string) ([]byte, error) {
	return f(key)
}

//-------------------------------------------------------------

//一个 Channel 可以认为是一个缓存的命名空间
type Channel struct {
	//每个 Channel 拥有一个唯一的名称
	name      string

	//缓存未命中时获取源数据的回调
	getter    Getter

	//每个Channel拥有一个并发缓存实例
	mainCache Cache

	peers     PeerPicker
}


// RegisterPeers registers a PeerPicker for choosing remote peer
func (c *Channel) RegisterPeers(peers PeerPicker) {
	if c.peers != nil {
		panic("RegisterPeerPicker called more than once")
	}
	c.peers = peers
}

var (
	rwmtx     sync.RWMutex
	channelMap = make(map[string]*Channel)
)


func NewChannel(name string, cacheBytes int64, getter Getter) *Channel {
	if getter == nil {
		panic("nil Getter")
	}
	rwmtx.Lock()
	defer rwmtx.Unlock()
	channel := &Channel{
		name:      name,
		getter:    getter,
		mainCache: Cache{cacheBytes: cacheBytes},
	}
	channelMap[name] = channel
	return channel
}

func GetChannel(name string) *Channel {
	rwmtx.RLock()
	defer rwmtx.RUnlock()
	channel := channelMap[name]
	return channel
}

//-------------------------------------------------------------

//从 mainCache 中查找缓存，如果存在则返回缓存值
//缓存不存在，则调用 load 方法, 封装后调用用户回调函数获取源数据，并添加到缓存

func (channel *Channel) Get(key string)(ByteView, error){
	if key == ""{
		return ByteView{}, fmt.Errorf("key is required")
	}
	if rst, ok := channel.mainCache.Get(key); ok {
		fmt.Println("[toyCache] hit " + key)
		return rst, nil
	} else {
		fmt.Println("[toyCache] miss " + key)
		return channel.load(key)
	}
}

func (channel *Channel) load(key string)(ByteView, error){
	//查找真实节点
	if channel.peers != nil {
		if peer, ok := channel.peers.PickPeer(key); ok {
			if value, err := channel.getFromPeer(peer, key); err == nil {
				return value, nil
			} else {
				log.Println("[GeeCache] Failed to get from peer", err)
			}
		}
	}

	//调用本地加载方法
	return channel.loadLocally(key)
}

//从远程缓存加载数据
func (channel *Channel) getFromPeer(peer PeerGetter, key string) (ByteView, error) {
	bytes, err := peer.Get(channel.name, key)
	if err != nil {
		return ByteView{}, err
	}
	return ByteView{b: bytes}, nil
}

//从本地缓存加载数据
func (channel *Channel) loadLocally(key string)(ByteView, error){
	rst, err := channel.getter.Get(key)
	if err == nil {
		bw := ByteView{b: cloneBytes(rst)}
		fmt.Println("[toyCache] loadLocally " + key)
		channel.mainCache.Add(key, bw)
		return bw, err
	} else {
		return ByteView{}, err
	}
}
