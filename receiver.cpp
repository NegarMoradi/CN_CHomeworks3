#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include "segment.hpp"

#define PORT 8080
using namespace std;

class ClientReceiver;
class Receiver {
public:
  	Receiver(int port, int router_port);
	int start();
	string get_file_size(string file_path);
	int setup_socket();
private:
  	int port;
  	int router_port;
	int sockfd;
  	struct sockaddr_in servaddr, router_addr;
	map<int, ClientReceiver*> clients;
};

Receiver::Receiver(int port, int router_port)
{
	this->port = port;
	this->router_port = router_port;
}


string Receiver::get_file_size(string file_path) { 
    ifstream in_file(file_path, ios::binary);
    in_file.seekg(0, ios::end);
    int file_size = in_file.tellg();
    return to_string(file_size);
}

int Receiver::setup_socket() {
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		cerr << "socket creation failed" << endl;
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&router_addr, 0, sizeof(router_addr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(port);

	// Filling server information
	router_addr.sin_family = AF_INET; // IPv4
	router_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	router_addr.sin_port = htons(this->router_port);

	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr,
					 sizeof(servaddr)) < 0)
	{
		cerr << "bind failed" << endl;
		exit(EXIT_FAILURE);
	}

}

int Receiver::start() {

	setup_socket();

	char incoming_buffer[SEGMENT_SIZE];
	char outgoing_buffer[SEGMENT_SIZE];

	struct sockaddr_in cliaddr;

	while (true)
	{
		memset(&cliaddr, 0, sizeof(cliaddr));
		memset(incoming_buffer, 0, sizeof(incoming_buffer));
		socklen_t cliaddr_len = sizeof(cliaddr);
		recvfrom(sockfd, incoming_buffer, SEGMENT_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr,
						 &cliaddr_len);

		auto segment = new Segment();
		segment->deserialize(incoming_buffer);

		int client_port = segment->get_src_port();

		if (clients.find(client_port) == clients.end())
		{
			clients[client_port] = new ClientReceiver(client_port, this->port);
		}

		memset(outgoing_buffer, 0, sizeof(outgoing_buffer));
		auto success = clients[client_port]->process_packet(segment, outgoing_buffer);

		if (success)
		{
			sendto(sockfd, outgoing_buffer, strlen(outgoing_buffer),
						 MSG_CONFIRM, (struct sockaddr *)&router_addr, sizeof(router_addr));
		}
	}
}


int main(int argc, char *argv[])
{
	auto server_port = stoi(argv[1]);
	auto router_port = stoi(argv[2]);

	Receiver receiver(server_port, router_port);

	receiver.start("1.png", "192.168.111.222.192.168.111.222 ");
}

