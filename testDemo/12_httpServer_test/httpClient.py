import json
import socket
import requests

# 目标 URL
url = "http://192.168.1.10:8080/index/getDeviceList"

# 发送 HTTP GET 请求
try:
    response = requests.get(url, timeout=5)

    # 检查请求是否成功（HTTP 状态码为 200）
    if response.status_code == 200:
        print("请求成功！")
        print("响应内容：", response.text)
        
    else:
        print(f"请求失败，状态码：{response.status_code}")
        print("响应内容：", response.text)  # 打印原始响应内容

except requests.exceptions.RequestException as e:
    print(f"请求发生异常：{e}")