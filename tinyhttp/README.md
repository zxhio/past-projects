### Tiny Http ###
`Tiny Http` 是一个迭代服务器，提供静态和动态的内容，可以将浏览器指向 `Tiny Http`，显示文本和图片的 `web` 界面
- `main` 函数通过调用 `open_listenfd` 打开一个监听套接字，监听命令行中传递的连接请求
    ``` C
    int open_listenfd(int port)
    {
        ...
        // 监听所有网卡 IP 地址
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        // 指定端口
        serveraddr.sin_port = htons((unsigned short)port);
        ...
    }
    ```
    进行无限迭代，不断接受连接请求，执行事务，关闭连接的那一端
    ``` C
    while (1) {
        connfd = Accept(listenfd, (struct sockaddr *)clientaddr, &clientlen);
        doit(connfd);
        Close(connfd);
    }
    ```
- `doit` 函数处理一个 `HTTP` 事务
    - 读取
        ``` C
        rio_readinitb(&rio, fd);
        rio_readlineb(&rio, buf, MAXLINE);
        sscanf(buf, "%s %s %s", method, uri, version); 
        ```
    - 解析请求行，调用 `parse_uri`
        ```C
        // 判断是否为支持的方法，只支持 GET，当不为 GET 时，发送错误信息，并返回 main，关闭连接
        if (strcasecmp(method, "GET") != 0)
        // 判断是否为静态文件
        is_static = parse_uri(uri, filename, cgiargs);
        // （静态）判断是否为常规文件及是否拥有读写权限
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
        // （动态）判断是否为常规文件及是否可执行
        if (!(S_ISREG(sbuf.st_mode) || !(S_IXUSR & sbuf.st_mode))) 
        ```
    - 根据上面的判断而选择具体的动静态服务

- `read_request_header` 仅读取请求报头并忽略这些报头
    ```C
    rio_readlineb(rp, buf, MAXLINE);
	while (strcmp(buf, "\r\n")) {
		rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
    ```

- `parse_uri` 将 `URI` 解析为一个文件名和一个可选的 `CGI`参数字符串，`Tiny Http` 静态内容的主目录
就是它当前目录 `./`, 可执行文件的主目录是 `./cgi-bin`，包含任何字符串 `cgi-bin` 的`URI`都会被认为
表示的是对动态内容的请求
    -  如果为静态内容
        ```C
        // 判断是否在 cgi-bin 目录下
        if (!strstr(uri, "cgi-bin"))
        // 清除 CGI 参数字符串
        strcpy(cgiargs, "");
        // 将 URI 转换为 Linux 相对路径名，eg： ./index.html
        strcpy(filename, ".");
        strcat(filename, uri);
        // 如果 URI 是已 "/" 结尾，把默认文件名加在后面
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "home.html");
        ```
    - 如果是动态内容
        ```C
        // 抽取所有的 CGI 参数
        p = index(uri, '?');
        if (p) {
            strcpy(cgiargs, p+1);
            *p = '\0';
        } else {
            strcpy(cgiargs, "");
        }
        // 将 URI 剩下的部分转换为一个相对文件名
        strcpy(filename, ".");
        strcat(filename, uri);
        ```
- `server_static` 发送一个 `HTTP` 响应，其主体包含一个本地文件的内容，`Tiny Http` 提供五种静态内容：
 `HTML 文件`，`无格式文件`，`GIF`，`PNG`，`JPG`格式的图片
    - 检查文件名后缀判断文件类型 `get_filetype`
    - 发送响应行和响应报头给客户端
        ```C
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
        sprintf(buf, "%sServer: Tiny Http Server\r\n", buf);
        sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
        sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
        rio_writen(fd, buf, strlen(buf));
        ```
    - 将请求文件的内容复制到已连接描述符 `fd` 来发送响应主体
        ```C
        // 只读打开
        srcfd = Open(filename, O_RDONLY, 0);
        // 将文件映射到一个虚拟内存空间
        srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
        Close(srcfd);
        // 复制从 srcp 位置开始的 filesize 到客户端已连接的描述符中
        rio_writen(fd, srcp, filesize);
        // 释放映射的虚拟内存空间
        munmap(srcp, filesize);
        ```
- `server_dynamic` fork 一个子进程运行 `CGI` 程序
    - 发送一个表明成功的响应行包括 Server 报头
        ```C
       	sprintf(buf, "HTTP/1.0 200 OK\r\n");
        rio_writen(fd, buf, strlen(buf));
        sprintf(buf, "Server: Tiny HTTP Server\r\n");
        rio_writen(fd, buf, strlen(buf));
        ```
    - fork 子进程，设置环境变量，重定向标准输出到已连接的文件描述符，加载运行 `CGI` 程序，
    且父进程阻塞 `wait`，当子进程终止时，回收子进程的资源

- `client_error` 包含相应的状态码和状态消息


### 运行 ###
1. make
2. ./httpserver 4096
3. 打开浏览器,在地址栏输入 `http://127.0.0.1:4096` 显示静态内容，`http://127.0.0.1:4096/cgi-bin/adder?1&2/` 显示动态内容

### TODO ###
添加多线程处理