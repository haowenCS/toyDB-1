package toycache

import (
	"io/ioutil"
	"log"
	"net/http"
	"net/url"
	"strings"
	"fmt"
	"sync"
	"consistenthash"
)

const (
	defaultBasePath = "/toycache/"
	defaultReplicas = 50
)

// HTTPPool implements PeerPicker for a pool of HTTP peers.
type HTTPPool struct {

	// this peer's base URL, e.g. "https://example.net:8000"
	self     		string
	basePath 		string

	mtx          	sync.Mutex // guards peers and httpGetters
	peersMap     	*consistenthash.Map
	//每个peer(真实节点)对应一个httpGetter
	httpGetters 	map[string]*httpGetter // keyed by e.g. "http://10.0.0.2:8008"
}

// NewHTTPPool initializes an HTTP pool of peers.
func NewHTTPPool(self string) *HTTPPool {
	return &HTTPPool{
		self:     self,
		basePath: defaultBasePath,
	}
}

// Log info with server name
func (p *HTTPPool) Log(format string, v ...interface{}) {
	log.Printf("[Server %s] %s", p.self, fmt.Sprintf(format, v...))
}

// ServeHTTP handle all http requests
func (p *HTTPPool) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	if !strings.HasPrefix(r.URL.Path, p.basePath) {
		panic("HTTPPool serving unexpected path: " + r.URL.Path)
	}
	p.Log("%s %s", r.Method, r.URL.Path)
	// /<basepath>/<channelname>/<key> required
	parts := strings.SplitN(r.URL.Path[len(p.basePath):], "/", 2)
	if len(parts) != 2 {
		http.Error(w, "bad request", http.StatusBadRequest)
		return
	}

	channelName := parts[0]
	key := parts[1]

	channel := GetChannel(channelName)
	if channel == nil {
		http.Error(w, "no such channel: "+channelName, http.StatusNotFound)
		return
	}

	rst, err := channel.Get(key)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/octet-stream")
	w.Write(rst.ByteSlice())
}

// 初始化（设置）真实节点
func (p *HTTPPool) Set(peers ...string) {
	p.mtx.Lock()
	defer p.mtx.Unlock()
	p.peersMap = consistenthash.NewMap(defaultReplicas, nil)
	p.peersMap.Add(peers...)
	p.httpGetters = make(map[string]*httpGetter, len(peers))
	for _, peer := range peers {
		p.httpGetters[peer] = &httpGetter{baseURL: peer + p.basePath}
	}
}

// PickPeer picks a peer according to key
func (p *HTTPPool) PickPeer(key string) (PeerGetter, bool) {
	p.mtx.Lock()
	defer p.mtx.Unlock()
	if peer := p.peersMap.Get(key); peer != "" && peer != p.self {
		p.Log("Pick peer %s", peer)
		return p.httpGetters[peer], true
	}
	return nil, false
}

var _ PeerPicker = (*HTTPPool)(nil)


type httpGetter struct {
	baseURL string
}

func (h *httpGetter) Get(channel string, key string) ([]byte, error) {
	u := fmt.Sprintf(
		"%v%v/%v",
		h.baseURL,
		url.QueryEscape(channel),
		url.QueryEscape(key),
	)
	res, err := http.Get(u)
	if err != nil {
		return nil, err
	}
	defer res.Body.Close()

	if res.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("server returned: %v", res.Status)
	}

	bytes, err := ioutil.ReadAll(res.Body)
	if err != nil {
		return nil, fmt.Errorf("reading response body: %v", err)
	}

	return bytes, nil
}

var _ PeerGetter = (*httpGetter)(nil) //啥意思

