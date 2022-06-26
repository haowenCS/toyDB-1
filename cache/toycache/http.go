package toycache

import (
	"fmt"
	"log"
	"net/http"
	"strings"
)

const defaultBasePath = "/toycache/"

// HTTPPool implements PeerPicker for a pool of HTTP peers.
type HTTPPool struct {

	// this peer's base URL, e.g. "https://example.net:8000"
	self     string

	basePath string
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