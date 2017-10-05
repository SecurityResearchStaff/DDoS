## UDP Amplification Attack
* 工具需运行支持IP欺骗服务的服务器上，否则不能发包；
* 利用UDP协议的天然脆弱点，即不需要前期建立连接，直接就可以向client发送数据；
* 互联网上存在大量分布式的UDP服务，利用服务的缺陷进行反射放大（具体原理Google搜索"反射放大攻击"）；
-- -

## 编译工具
* Centos
```
$ yum -y install gcc
```

* Ubuntu
```
$ apt-get install gcc
```

-- -
本代码只提供学习和压力测试，如用将该代码于非法用途所导致的后果自负，与本人无关...