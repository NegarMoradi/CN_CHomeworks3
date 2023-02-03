#include "segment.hpp"

#include <vector>
#include <queue>
#include <iostream>
#include <map>
#include <random>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

using namespace std;

class Router {
public:
	Router(int router_port, int queue_size, int drop_rate);
	void run();
	void run_socket();
	int process_incoming();
	int process_outgoing();
	void add_to_queue(Segment *segment);
	bool is_random_drop();
private:
	queue<Segment *> segment_queue;
  	int router_port;
	int sender_port;
	int receiver_port;

 	int queue_size;
  	int drop_rate;

  	map<int, int> port_map;

  	int sockfd;
  	struct sockaddr_in send_addr, routeraddr;

	int ssthresh;
};

Router::Router(int _router_port, int _queue_size, int drop_rate)
{
	this->router_port = _router_port;
	this->queue_size = queue_size;
	this->drop_rate = drop_rate;

	srand(time(NULL));
}

void Router::run_socket()
{
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation:");
		exit(EXIT_FAILURE);
	}

	memset(&send_addr, 0, sizeof(send_addr));
	this->send_addr.sin_family = AF_INET;
	this->send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	memset(&routeraddr, 0, sizeof(routeraddr));

	// Filling router information
	routeraddr.sin_family = AF_INET; // IPv4
	routeraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	routeraddr.sin_port = htons(router_port);

	// Bind the socket with the router address
	if (bind(sockfd, (const struct sockaddr *)&routeraddr,
					 sizeof(routeraddr)) < 0)
	{
		perror("bind failed:");
		exit(EXIT_FAILURE);
	}
}

void Router::run()
{
	run_socket();
	process_incoming();

	// thread incoming_thread(&Router::process_incoming, this);
	// thread outgoing_thread(&Router::process_outgoing, this);

	// incoming_thread.join();
	// outgoing_thread.join();
}

int Router::process_incoming()
{
	struct sockaddr_in incomming_addr;
	int nbytes;
	socklen_t incomming_addr_len = sizeof(incomming_addr);

	char buffer[HEADER_SIZE];

	while (true)
	{
		memset(&buffer, 0, HEADER_SIZE);
		nbytes = recvfrom(sockfd, buffer, HEADER_SIZE, 0, (struct sockaddr *)&incomming_addr, &incomming_addr_len);
		if (nbytes < 0)
		{
			perror("Recvfrom: ");
			close(sockfd);
			exit(1);
		}

		auto segment = new Segment();
		segment->deserialize(buffer);

		if (is_random_drop())
			cout << "Segment with seg_id:ack " << segment->get_seg_id() << ":" << segment->get_ack() << " random dropped" << endl;
		else
		{
			add_to_queue(segment);
			cout << "Segment with seg_id:ack " << segment->get_seg_id() << ":" << segment->get_ack() << " received from " << ntohs(incomming_addr.sin_port) << endl;
		}
	}
}

int Router::process_outgoing()
{
	while (true)
	{
		auto segment = this->segment_queue.front();
		this->segment_queue.pop();

		// send with socket
		char buffer[HEADER_SIZE];
		memset(buffer, 0, HEADER_SIZE);
		segment->serialize(buffer);

		struct sockaddr_in send_addr;
		memset(&send_addr, 0, sizeof(send_addr));
		send_addr.sin_family = AF_INET;
		send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		send_addr.sin_port = this->port_map[segment->get_dst_port()];

		auto nbytes = sendto(this->sockfd, buffer, strlen(buffer), MSG_CONFIRM,
												 (const struct sockaddr *)&send_addr, sizeof(send_addr));

		cout << "Segment with seq_num:ack " << segment->get_seg_id() << ":" << segment->get_ack() << " sent to " << ntohs(send_addr.sin_port) << endl;
		// printf("\n\n%s\n\n\n", buffer);
	}
}

void Router::add_to_queue(Segment *segment)
{
	if (segment_queue.size() == queue_size)
	{
		cout << "Segment with seq_num:ack " << segment->get_seg_id() << ":" << segment->get_ack() << " buffer dropped" << endl;
		return;
	}
	segment_queue.push(segment);
}

bool Router::is_random_drop()
{
	auto dropped = rand() % 100 < drop_rate;
	return dropped;
}

int main(int argc, char *argv[])
{
	auto router_port = stoi(argv[1]);

	int queue_size = 5;
	int drop_rate = 0;

	Router router(router_port, queue_size, drop_rate);

	router.run();
}

