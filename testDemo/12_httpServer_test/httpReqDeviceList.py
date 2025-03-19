import json
import time
import socket
import requests

# 发送 HTTP GET 请求
try:
    # 获取设备列表
    url_get_device_list = "http://192.168.3.234:8080/index/getDeviceList"
    
    response_get_device_list = requests.get(url_get_device_list, timeout=5)
    
    if response_get_device_list.status_code == 200:
        print("请求成功！")
        print("响应内容：", response_get_device_list.text)
        
    else:
        print(f"请求失败，状态码：{response_get_device_list.status_code}")
        print("响应内容：", response_get_device_list.text)

except requests.exceptions.RequestException as e:
    print(f"请求发生异常：{e}")