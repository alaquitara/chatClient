INSTRUCTIONS FOR USE OF ASSIGNMENT1:

COMPILE:
	1. Save zip file to a directory of your choosing
	2. CD to directory with contents of the file
	3. call unzip project1.zip
	4. While still in directory containing all files type <make> into the console
	5. Console should read back: gcc chatClient.c -o chatClient -Wall -std=c99 -D_GNU_SOURCE ...This means the chat client has compiled properly
EXECUTE:
	1. In the same directoy you used to unzip the files enter command python chatserve.py localhost <portnumber>.  Use a non-regular port number in the <portnumber> section and be sure to drop the <>.
	2. Your shell should display "Listening for connections on  ('127.0.0.1', <portnumber>)".  Don't worry if the numbers don't match the example.
	3. Open another shell. (If using flip, be sure to be on the same flip server as the chatserver. e.g flip1).
	4. Navigate to the directory where all the files have already been unzipped.
	5. Since we have already compiled the client, we can simply use the command ./chatClient localhost <port number of chat server>.  Portnumber in this case is the same exact number you used to start the chatserver.
	6. If the steps were followed correctly, the shell running the client should read "Connected to the remote host".
CONTROL:
	1.  At this point the client will ask "Enter a handle you wish to use:".  The handle can be any name you want up to 10 characters long.
	2.	Now we are up and running and able to exchange messages from client to server and back.
	3.  In the client window at each iteration it will say "<Handle> > Enter a message or '\quit' to end: "
	4.	The client can now send any message they want up to 500 characters to the server by typing in the message and hitting enter.
	5.	The server will receive the message from the client in the form <handle> > "<message contents>".
	6.	At this point the server can respond with a message or close the connection with \quit.  To respond, just follow steps 3 and 4 in the server window.
	7.  This parlay will continue back and fourth, taking turns sending messages from server to client and vice versa until one of them enters \quit and closes the connection.

