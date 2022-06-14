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
  * 使用ValueObject封装指向value的指针，目前可以作为value的类型有string、double、string_list、double_list
  * 使用union以节省空间，如果value是double或其它简单类型(和void\*所占空间相同)，就直接存储在union里，否则会存储指向value的指针。但要注意析构，避免内存泄露
  ```c
  /*database/toydb.h*/
  enum ValueType{
    NONE = 0,
    DOUBLE,
    STRING,
    DOUBLE_LIST,
    STRING_LIST,
  };
  
  class ValueObject{
  public:
    ...//省略了构造和析构函数
    ValueType value_type;
    union{
       double double_type;
       void* ptr_type; 
    }value;
  };
  ```
* K-V存储系统开放了insert_element, delete_element, has_element, get_element接口
  * 在client端分别使用`set key value`和`del key`和`has key`和`get key`来调用服务
  * server端执行相应的命令，并根据执行结果返回status
  * 若没有找到key, 会返回`NIL`, 插入成功返回`OK`
  * 若以列表作为value，列表中的全部值应为同一种类型，如：全为double或全为string
  * 若插入字符作为value，应被双引号`""`包围，否则暂认为是double
* 使用protobuf作为序列化\反序列化工具，在server和client之间传递指令

![1655221087(1)](https://user-images.githubusercontent.com/75946871/173618579-683e5389-ab6d-4ebd-98c1-94987b0b5dd6.png)


TODO:
* 增加持久化方法
* 仿照redis为存储系统建立更多的数据结构
   * 如，将sds引入项目，取代char[]
* 可以加入多种语言编写的客户端，如python、java、go
* 增加一些模板编程的技巧
* 目前仅支持std::string类型的key
* 加入垃圾回收机制
* 目前还是单机数据库，无论是稳定性还是存储能力都有明显的瓶颈，可以尝试往多机数据库方向发展
