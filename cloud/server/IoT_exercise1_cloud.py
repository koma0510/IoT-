#! /usr/bin/env python
# coding:utf-8
# tcp_server

import socket
import threading
import csv
from datetime import datetime

bind_ip = '0.0.0.0'
bind_port = 10340
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(5)
print('[*] Listening on %s:%d' % (bind_ip, bind_port))

# データのバリデーション
def validate_data(device_id, timestamp, illuminance, sensor_state):
    # device_idは0~3の数字
    if not device_id.isdigit() or not (0 <= int(device_id) <= 3):
        return False
    
    # timestampはYYYY-MM-DDTHH:mm:ssの形
    try:
        datetime.strptime(timestamp, '%Y-%m-%dT%H:%M:%S')
    except ValueError:
        return False
    
    # 照度は正の整数
    if not illuminance.isdigit() or int(illuminance) <= 0:
        return False
    
    # 人感センサは0 or 1
    if sensor_state not in ['0', '1']:
        return False
    
    return True

def save_to_csv(data):
    with open('../data/IoT_exercise1_received_data.csv', mode='a') as file:
        writer = csv.writer(file)
        writer.writerow(data)

def handle_client(client_socket):
    try:
        # データを受信
        data = client_socket.recv(1024).decode('utf-8')
        print('[*] Received: %s' % data)
        
        # データをカンマで分割
        data_parts = data.split(',')
        if len(data_parts) == 4:
            device_id, timestamp, illuminance, sensor_state = data_parts
            
            # データのバリデーション
            if validate_data(device_id, timestamp, illuminance, sensor_state):
                save_to_csv(data_parts)
                client_socket.send("OK\n".encode('utf-8'))
            else:
                print("Data validation failed.")
                client_socket.send("ERROR\n".encode('utf-8'))

        else:
            print("Received data format is incorrect.")
            client_socket.send("ERROR\n".encode('utf-8'))        
    except Exception as e:
        print("Error{}".format(e))
    finally:
        client_socket.close()

while True:
    client, addr = server.accept()
    print('[*] Accepted connection from: %s:%d' % (addr[0], addr[1]))
    client_handler = threading.Thread(target=handle_client, args=(client,))
    client_handler.start()