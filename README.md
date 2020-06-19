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
目前只支持linux环境

php --version
PHP 7.3.5 (cli) ( NTS )
注意这里是NTS版的php，目前nmid-php-ext支持php7.3及以上NTS非线程安全版php，不支持ZTS线程安全版php。

go version
go1.12.5 linux/amd64

gcc --version
gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-36)

cmake --version
cmake version 3.11.4

```

## 编译安装步骤

```
首先需要安装msgpack php扩展。

git clone https://github.com/HughNian/nmid-php-ext.git

1.client扩展
cd nmid-php-ext/clientext
phpize
./configure --with-php-config=/usr/local/php/bin/php-config //这个路径根据自己情况填写
make
make test //可不执行
make install

2.worker扩展
cd nmid-php-ext/workerext
phpize
./configure --with-php-config=/usr/local/php/bin/php-config //这个路径根据自己情况填写
make
make test //可不执行
make install

最后在php.ini文件中加入这两个扩展

```

## 使用

```php
工作端代码

nmid调度服务端ip:192.168.1.176
nmid调度服务端port:6808

$worker = new WorkerExt();
$worker->add_server("192.168.1.176", 6808);
function GetOrderInfo($params) {
    var_dump($params);
    $arr = array('sn'=>'MBO22933822', 'user_id' => 1);
    return json_encode($arr);
}

$worker->add_function('GetOrderInfo', function($params) {
    var_dump($params);
    return "goods goods very good";
});

$str = " is very best";
$worker->add_function('ToUpper', function($params) use ($str) {
    var_dump($params);
    return strtoupper($params[0]) . $str;
});

$worker->add_function('PostInfo', function($params){
    var_dump($params);
    return "post ok";
});

//$worker->del_function('PostInfo');

//$worker->add_function('GetOrderInfo', "GetOrderInfo");

$worker->run();

```

```php
客户端代码

nmid调度服务端ip:192.168.1.176
nmid调度服务端port:6808

$host = "192.168.1.176";
$port = 6808;

$client = new ClientExt($host, $port);
$client->connect();

$params = msgpack_pack(array("name:niansong"));
$client->dowork("PostInfo", $params, function($ret){
	var_dump($ret);
});

$params2 = msgpack_pack(array("order_sn:MBO993889253", "order_type:4", "fenxiao:2288"));
$client->dowork("GetOrderInfo", $params2, function($ret){
	var_dump($ret);
});

$client->close();

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
