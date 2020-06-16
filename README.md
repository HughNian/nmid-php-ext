<div align="center">
    <a href="http://www.niansong.top"><img src="https://raw.githubusercontent.com/HughNian/nmid-php-ext/master/logo/nmid_php_ext_logo.png" alt="nmid Logo" width="160"></a>
</div>

## nmid-php-ext介绍

nmid-php-ext是nmid微服务调度系统的客户端和工作端的php扩展实现。采用的是nmid-c的C动态库。目前client和worker是两个独立的扩展装入php中的，后期会合并成一个扩展。

1.clientext目录为客户端php扩展源码目录，主要用到nmid-c的客户端动态库     

2.workerext目录为工作端php扩展源码目录，主要用到nmid-c的工作端动态库         


## 建议配置

```
cat /proc/version
Linux version 3.10.0-957.21.3.el7.x86_64 ...(centos7)

php --version
PHP 7.3.5 (cli) ( NTS )
注意这里是NTS版的php，目前nmid-php-ext支持php7.3及以上NTS非线程安全版php。

go version
go1.12.5 linux/amd64

gcc --version
gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-36)

cmake --version
cmake version 3.11.4

```

## 编译安装步骤

```
https://github.com/HughNian/nmid-php-ext.git

1.client扩展
cd nmid-php-ext/clientext
phpize
make
make install

2.worker扩展
cd nmid-php-ext/workerext
phpize
make
make install

最后在php.ini文件中加入这两个扩展

```

<div align="center">
    <a href="http://www.niansong.top"><img src="https://raw.githubusercontent.com/HughNian/nmid-php-ext/master/logo/clientext_logo.png" alt="clientext"></a>
</div>
<br>
<div align="center">
    <a href="http://www.niansong.top"><img src="https://raw.githubusercontent.com/HughNian/nmid-php-ext/master/logo/workerext_logo.png" alt="workerext"></a>
</div>


## 交流博客

http://www.niansong.top