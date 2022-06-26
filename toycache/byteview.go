package toycache

//封装了byte切片
type ByteView struct {
	//存储真实的缓存值
	//选择 byte 类型是为了能够支持任意的数据类型的存储，例如字符串、图片等
	b []byte
}

func (bv ByteView) Len() int {
	return len(bv.b)
}

//b 是只读的，使用 ByteSlice() 方法返回一个拷贝，防止缓存值被外部程序修改。
func (bv ByteView) ByteSlice() []byte {
	return cloneBytes(bv.b)
}

func (bv ByteView) String() string {
	return string(bv.b)
}

func cloneBytes(b []byte) []byte {
	c := make([]byte, len(b))
	copy(c, b)
	return c
}