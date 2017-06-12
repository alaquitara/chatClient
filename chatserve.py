"""
Alexander Laquitara
CS 372 project 1
Description: Design and implement a simple chat system that works for one pair of users, i.e., create two programs: a chat server and a chat client.
"""
import socket
import sys
import os

#function called in main for server operations.  Takes a valid client socket as a parameter

def chatServ(clientSocket):
	print "waiting for client connection\n"
	quit = False
	while(quit == False):
		#  used to receive  messages from a socket.  It retuns the length of the message on successful completion
		# referenced http://man7.org/linux/man-pages/man2/recv.2.html  http://beej.us/guide/bgnet/output/html/multipage/recvman.html
		received = clientSocket.recv(511)
		if(received == 0):
			print "messaged not received from client\n"
			break
		if(received == "\quit"):
			print "client closed the connection\n"
			quit = True
			clientSocket.close()
			break			

		print received

		isValid = False
		while(isValid != True):
			sentMessage = raw_input("Enter a message or enter command '\quit' to close connection: ")
			if(len(sentMessage) < 500):
				isValid = True
		if(sentMessage == "\quit"):
			print 'connection closed by server'
			clientSocket.send(sentMessage)
			quit = True
			clientSocket.close()
			return -1;
		else:
			clientSocket.send(sentMessage)
			print "waiting for client response...\n"

def main():
	#create a socket using AF_INET and SOCK_Stream from lecture
	serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
	# AF_INET sockets this means that a socket may bind, except when there is an active listening socket bound to the address.
	#referenced http://man7.org/linux/man-pages/man7/socket.7.html , http://pubs.opengroup.org/onlinepubs/009695399/functions/setsockopt.html
	serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	# 2nd agrument while running from shell will be the server number
	serverPort= int(sys.argv[2])
	serverAddress = (socket.gethostbyname(sys.argv[1]), serverPort)
	#bind the socket to the server address
	serverSocket.bind(serverAddress)
	#listening for connections - from lecture
	serverSocket.listen(1)
	print "Listening for connections on  " + str(serverAddress)
	
	serverOpen = True
	while(serverOpen == True):

		# Accept a connection. The socket must be bound to an address and listening for connections. The return value is a pair (conn, address) where conn is a new socket 
		# reference https://docs.python.org/2/library/socket.html
		clientSocket, clientAddress = serverSocket.accept()
		print "connection made with client " + str(clientAddress)
		
		n = chatServ(clientSocket)
		if(n == -1):
			serverOpen = False
		else:
			print "Listening for connections on " + str(serverAddress)
	serverSocket.close()
main()