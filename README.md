toyDB

最近在工作中使用bazel比较多，于是就趁热用bazel构建了这个toyDB项目

参照游双《Linux高性能服务器编程》和redis的一些思想，实现了这个小玩具

目前实现的功能：

* epoll多路复用IO，使用reactor模式
  * reactor + acceptor + handler
* C++11风格的线程池，使用条件变量实现阻塞队列，利用std::function和std::bind将任务函数绑定添加到线程池
* 利用SIGALRM和堆，关闭长时间不活跃的连接
* 添加了一些基于gtest的单元测试
* 基于跳表实现了简易的K-V存储系统
 * 目前实现了: skiplist, sortlist
 * 开放了insert_element, delete_element, has_element, get_element接口
* 使用protobuf作为序列化\反序列化工具，在server和K-V database之间传递指令

TODO:
* 仿照为存储系统建立更多的数据结构
   * 如，将sds引入项目，取代char*
* 可以加入多种语言编写的客户端，如python、java、go
* 增加一些模板编程的技巧
