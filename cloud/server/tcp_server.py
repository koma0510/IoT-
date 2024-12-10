#! /usr/bin/env python
# coding:utf-8
# tcp_server

import socket
import threading

bind_ip = '0.0.0.0'
bind_port = 10340 # ** 割り当てられたものを使用せよ**
server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.bind((bind_ip,bind_port))
server.listen(5)
print '[*] listen %s:%d' % (bind_ip,bind_port)

def handle_client(client_socket):
  bufsize=1024
  request = client_socket.recv(bufsize)
  print '[*] recv: %s' % request
  client_socket.send("Hey Client!\n")
  client_socket.close()

while True:
  client,addr = server.accept()
  print '[*] connected from: %s:%d' % (addr[0],addr[1])
  client_handler = threading.Thread(target=handle_client,args=(client,))
  client_handler.start()