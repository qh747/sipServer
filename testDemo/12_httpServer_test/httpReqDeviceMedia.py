import json
import time
import socket
import requests

# 发送 HTTP GET 请求
try:  
    # 请求设备媒体
    url_req_device_meida = "http://192.168.3.234:8080/index/reqDeviceMedia/10000000001310000059"
    
    header_req_device_meida = {
        "Content-Type": "application/json"
    }
    
    body_req_device_meida = {
        "deviceId": "10000000001310000059",
        "playType": "play",
        "protoType": "udp"
    }
    
    response_req_device_meida = requests.post(url_req_device_meida, headers=header_req_device_meida, data=json.dumps(body_req_device_meida), timeout=5)

    if response_req_device_meida.status_code == 200:
        print("请求成功！")
        print("响应内容：", response_req_device_meida.text)
        
    else:
        print(f"请求失败，状态码：{response_req_device_meida.status_code}")
        print("响应内容：", response_req_device_meida.text)

except requests.exceptions.RequestException as e:
    print(f"请求发生异常：{e}")
    