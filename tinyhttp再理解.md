tinyhttp理解：

### 第一步，绑定http服务

**socket**

功能：建立套接字，实现通信

```c
int socket(int domain, int type, int prorocol);
```

domain参数的作用：

确定通信方式，这个版本中是ipv4，一般是AF_INET

type参数的作用：

确定数据传输方式，一般使用SOCK_STREAM和SOCK_DGRAM，SOCK_STREAM是数据流式协议，一般是TCP通信的时候使用

SOCK_DGRAM是数据报式传输协议，一般是UDP通信的时候使用。

prorocol参数的作用：

表示传输协议，TCP/UDP，一般指定type的时候，这里可以省略不写

返回值：

文件描述符

 **setsockopt**

功能：设置socket相关的属性，在tinyhttp中的作用是端口复用

```c
int socketopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
```

sockfd参数的作用：

确定要对哪个文件描述符进行修改

level参数的作用：

选项定义的层次，一般使用SOL_STREAM

optname参数的作用：

需要设置的选项，在这里使用的是端口复用，SO_REUSEADDR

**getsockname**

功能：可以获取一个与socket相关的地址，可以用来动态的获取端口号

```c
int getsockname(int sockfd, struct sockaddr *addr, socklen_t * addrlen);
```

sockfd参数的作用：

需要获取信息的套接字

addr参数的作用：

传出参数，获取信息

 setsocket（设置属性），

**bind**

功能：把一个本地套接字地址赋给一个套接字,如果没有设置的话，内核要为套接字选一个临时端口

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

sockfd参数的作用：

要把本地套接字地址赋给哪个套接字

addr参数的作用：

一般是先给sockaddr_in类型定义，然后强制转换成sockaddr类型

地址族的功能是：确定底层使用哪种通信协议来递交数据

AF_INET用的是TCP/IPV4

```c
struct sockaddr_in{
	sa_family_t sin_family;  // 地址族，一般为AF_INET
    uint16_t sin_port; //  16位TCP/UDP端口号
    struct in_addr sin_addr; // 32位IP地址
    char sin_zero[8];
};

struct in_addr{
    in_addr_t s_addr;
};
```

注意这里的sin_port和sin_addr都应该将本地字节序转换为网络字节序

网络字节序是大端存储，也就是低地址存储高位，高地址存储低位

```c
uint32_t htonl(uint32_t hostlong);
将一个无符号短整型数值转换为网络字节序
uint16_t htons(uint16_t hostshort);
将一个无符号行长整型数转换成网络字节序
uint32_t ntohl(uint32_t netlong);
将一个无符号短整型数值转换成本地字节序
uint16_t ntohs(uint16_t netshort);
将一个无符号长整型数值转换成本地字节序
```

```c
int inet_pton(int af, const char *src, void *dst);
将字符串类型的IP地址转换为网络字节序
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
将IP类型的网络字节序转换为本地的套接字
```

listen（设置监听上限），

```c
int listen(int sockfd, int backlog);
```

参数：

sockfd代表套接字

backlog代表监听的上限



#### 在这第一步中，首先完成了对套接字的一系列初始化，设置传输方式（流/报），设置本地端口号（如果没有就动态随机分配一个端口号），允许哪些IP接入，bind设置监听上限。



### 第二步

**accpet**

作用：等待客户端发来建立连接请求

```c
int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
```

参数socket的作用：

哪个套接字和客户端进行通信

参数address的作用：

保存和socket请求相连的客户端的地址信息

参数address_len的作用：

保存长度



**pthread_create**

作用：服务器端新建一个线程去处理当前的这个http请求

```c
int pthread_create(pthread *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
```

参数pthread的作用：

返回新的线程id

参数attr的作用：

设置线程属性，在这个tinyhttp中，设置为NULL

第三个参数的作用：

线程运行函数的起始地址

第四个参数的作用：

线程运行函数的参数



#### 在这一步中，服务器端收到客户端发来的http请求，新建一个线程来处理http请求



### 第三步

对发送过来的http请求进行解析，判断发送过来的报文的请求方式（GET/POST），格式化出URL（统一资源定位符）到path数组。

path数组表示的是浏览器请求的服务器文件路径。在tinyhttp中是存储在htdocs文件夹下

**ps：**htdocs文件（host documents），主机文件，host文件主要作用是定义IP和主机名的映射关系

当url以  / 结尾或者文件目录结尾，在后面加上 index.html, 表示访问主页。

如果是无效的GET请求或者是url无执行权限，直接把服务器文件发送过去，断开连接。其他请求就继续去执行cgi脚本

HTTP请求报文格式：

请求行，请求头部，空格，请求数据

 ![img](https://upload-images.jianshu.io/upload_images/17980024-51c4180cf2a2c7b6.png?imageMogr2/auto-orient/strip|imageView2/2/w/466/format/webp) 

注意：

当请求方法为GET的时候，会有这种情况，将请求参数放在URL的后面，第一个参数之前使用 ？

如果请求方式为GET的时候，并且有？，query_string 指向的是？后面的GET参数

**get_line函数**

```c
int get_line(int fd, char *, int len);
```

作用是读取套接字的一行，把回车换行等情况都统一转换为换行符结束

**strcasecmp函数**

```c
int strcasecmp(const char *s1, const char *s2);
```

作用是忽略大小写实现字符串的比较

当s1 == s2返回0. 当s1大于s2则返回 >0 的数,其次返回小于0的数

**strcat函数**

字符串的拼接

**stat函数**

```c
int stat(const char *pathname, struct stat *statbuf);
```

获取pathname对应的文件信息

```c
struct stat{
    dev_t st_dev;  //文件的设备编号
    ino_t st_ino;  //节点
    mode_t st_mode; //文件的类型和存取权限
    nlink_t st_nlink; //硬链接数目
    uid_t st_uid; //用户ID
    gid_t st_gid; //组ID
    dev_t st_rdev; //如果这个文件为设备编号，该变量为设备编号
    off_t st_size; //文件字节数
    blksize_t st_blksize; //块大小（文件系统的I/O缓冲区大小）
    blkcnt_t st_blocks; //块数
    struct timespec st_atim;//最后一次访问的时间
    struct timespec st_mtim;//最后一次修改的时间
    struct timespec st_ctim;//最后一个改变的时间（属性）
}
```

**server_file函数**

```c
int server_file(int client, const char *buf);
```

向client对应的套接字中写入buf数组

#### 在这一步中，服务器解析客户端发送过来的请求报文，取出method（GET/POST），获取url。判断路径是否合法，url是否可执行，路径不合法，返回404.如果是路径合法的话，如果url不可执行或者无参数的GET请求，直接将path对应的服务器文件通过http格式发送过去，断开连接。如果路径合法，并且无上述非法情况，就去执行cgi脚本。



#### 第四步

首先读取所有的http请求，如果是POST的话，读取Content-Length，这个代表的是HTTP消息实体的传输长度

然后创建两个管道，一方面用于脚本的执行，另一方方面是将脚本的执行结果经过修饰返回到客户端

首先是管道的建立，通过pipe函数创建

管道1是cgi_output[2]， 管道2是cgi_output[2]。

STDIN 0

STDOUT 1 

STDERROR  2 

cgi_output[2]中，cgi_output[0] 代表数组的读取端，cgi_output[1] 代表数组的写入端。

接下来是服务器端向浏览器返回响应报文。首先将报文头部发送过去，状态码200

首先是管道中对应的重定向，将cgi_input的读取端重定向到STDIN，将cgi_output的写入端重定向到STDOUT。

  这样，在子进程中实现环境变量的设置，以及cgi脚本的执行（通过execl函数）

  在父进程中，将子进程执行的结果返回到客户端，然后回收子进程。

**dup2函数**

```c
int dup2(int oldfd, int newfd);
```

函数作用：newfd赋值old参数的文件描述符

如果参数newfd为一个打开的文件描述符，则newfd所指的文件会先被关闭。如果相等的话，则返回newfd

返回值：如果成功返回新的文件描述符

**execl函数**

```c
int execl(const char *path, const char *arg,.....)
```

path表示指针索要指向的文件路径，接下来的参数代表执行这个文件的时候传递的参数



#### 在这一步中，主要是实现cgi脚本，并且通过管道将返回结果返回到浏览器。

#### 在子进程中，通过execl函数调用cgi脚本，然后将输出结果写到管道中。在父进程中，通过管道读取数据，并且将数据回写到客户端中。

#### 注意：GET请求没有请求数据，是用于查询的

