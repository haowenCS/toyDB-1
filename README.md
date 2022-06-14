toyDB

为何使用bazel？快速构建、支持构建多种语言、便于扩展、结构清晰

为何使用protobuf？性能优秀、支持多种编程语言、简洁易懂


参照游双《Linux高性能服务器编程》和redis的一些思想，实现了这个小玩具

代码结构：
```
toyDB-+-server-+-BUILD
      |        |-服务端组件
      |-client-+-BUILD
      |        |-客户端组件
      |-database-+-BUILD
      |          |-kv数据库组件
      |-protoc-+-BUILD
      |        |-通信协议
      |-test-+-BUILD
      |      |-测试用例
      |-WORKSPACE
      +-bazel-bin生成的可执行文件
```


目前实现的功能：

* epoll多路复用IO，使用reactor模式
  * reactor + acceptor + handler
* C++11风格的线程池，使用条件变量实现阻塞队列，利用std::function和std::bind将任务函数绑定添加到线程池
* 利用SIGALRM和堆，关闭长时间不活跃的连接
* 添加了一些基于gtest的单元测试
* 基于skiplist和std::unordered_map实现了简易的K-V存储系统，后期可以考虑增加多种存储结构
  * key为string
  * value为包含了某种类型的对象，目前可以作为值的类型有string、double、string_list、double_list，但这些类型的设计对使用者透明，程序会自动判断
* K-V存储系统开放了insert_element, delete_element, has_element, get_element接口
  * 在client端分别使用`set key value`和`del key`和`has key`和`get key`来调用服务
  * server端执行相应的命令，并根据执行结果返回status
  * 若没有找到key, 会返回`NIL`, 插入成功返回`OK`
  * 若以列表作为value，列表中的全部值应为同一种类型，如：全为double或全为string
  * 若插入字符作为value，应被双引号`""`包围，否则暂认为是double
* 使用protobuf作为序列化\反序列化工具，在server和client之间传递指令

![N8@YWH5WCK}ZKUDG67YE}P9](https://user-images.githubusercontent.com/75946871/173413665-d06bcaeb-c0eb-48e7-ad34-7264659d2c40.png)


TODO:
* 仿照redis为存储系统建立更多的数据结构
   * 如，将sds引入项目，取代char[]
* 加锁，支持并发
* 可以加入多种语言编写的客户端，如python、java、go
* 增加一些模板编程的技巧
* 目前仅支持std::string类型的key
* 加入垃圾回收机制
* 目前还是单机数据库，无论是稳定性还是存储能力都有明显的瓶颈，可以尝试往多机数据库方向发展
