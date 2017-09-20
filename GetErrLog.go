/* 
 * 异常日志收集
 * 获取客户端请求传入的参数，URL特殊字符处理直接使用Base64加密请求访问
 * 服务端解密后写入日志文件，并且打印输出解密后的参数数据
 * 
 * Go run GetErrLog.go
 * http://localhost/logger_handler?value=SGVsbG8sV3JvbGQ
 */

package main

import (
	"encoding/base64"
	"net/http"
	"strings"
	"time"
	"fmt"
	"log"
	"os"
)

func encodeString(str string) string {
	encode_buf := base64.URLEncoding.EncodeToString([]byte(str))
	return encode_buf
}

func decodeString(encode_buf string) (decode_buf string, err error) {
	decode_bytes, err := base64.StdEncoding.DecodeString(encode_buf)
	if err != nil {
		return decode_buf, err
	}
	return string(decode_bytes), nil
}

func logger_to_file(logger_msg string) {
	tNow := time.Now()
	timeNow := tNow.Format("2006-01-02")
	logfile, err := os.OpenFile("logger_" + timeNow + ".txt", os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)
	if err != nil {
		fmt.Printf("%s\r\n", err.Error())
		return
	}
	defer logfile.Close()

	logger := log.New(logfile, "", log.Ldate|log.Ltime)
	logger.Println(logger_msg)
}

func logger_handler(w http.ResponseWriter, r *http.Request) {
	de_msg := r.FormValue("value")o
	ip := strings.Split(r.RemoteAddr, ":")
	if de_msg == "" {
		return
	}

	decode_res, err := decodeString(de_msg)
	if err != nil {
		fmt.Println(err.Error())
		return
	}
	
	fmt.Println("ClientIp = " + ip[0] + " " + decode_res + "\n")
	logger_to_file("ClientIp = " + ip[0] + " " + decode_res)
}

func main() {
	http.HandleFunc("/logger_handler", logger_handler)	// 设置访问的路由
	err := http.ListenAndServe(":80", nil)	// 设置监听端口
	if err != nil {
		fmt.Println("Listen Failed.")
	}
}