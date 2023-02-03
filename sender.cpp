#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "segment.hpp"

using namespace std;

const int TIME_OUT_LENGTH = 1;

#define NOT_SENT 0
#define SENT 1
#define RECEIVED 2

#define INFINITY 1000

class Sender
{
public:
  Sender(int sender_port, int receiver_port, int router_port);
  void start(string file_location);

private:
    int sender_port;
    int receiver_port;
    int router_port;

    struct sockaddr_in router_addr;
    int sockfd;

    int cwnd;
    int ssthresh;
    bool is_congest;

    vector<int> segments_status;

    void run_socket();
    vector<int> receive_ack();

    bool still_sending();
    vector<int> initialize_segments_status(int size);
    void set_cwnd();
    void send_bulk(vector<Segment> segments);
    void send_segment(Segment segment);
    bool has_segment_expired(Segment segment);
    void update_status(vector<bool> acks, vector<Segment> segments);
    bool should_wait();
    vector<Segment> choose_segments_to_send(vector<Segment> segments);
    vector<Segment> slice_file(string file_location);
};

Sender::Sender(int sender_port, int receiver_port, int router_port)
{
  this->sender_port = sender_port;
  this->receiver_port = receiver_port;
  this->router_port = router_port;

  this->cwnd = 1;
  this->ssthresh = INFINITY;
  is_congest = false;
}

void Sender::run_socket()
{
  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&router_addr, 0, sizeof(router_addr));

  // Filling server information
  router_addr.sin_family = AF_INET;
  router_addr.sin_port = htons(this->router_port);
  router_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
}

//checks the segments_status to see if we still need to send segments, if all of them are RECEIVED then file transmission is done
bool Sender::still_sending()
{

    for (int i=0 ; i<this->segments_status.size() ; i++) {
        if(this->segments_status.at(i) != RECEIVED) {
            return true;
        }
    }
    return false;
}

// sends the chosen segments to router and checks if they are less than cwnd
void Sender::send_bulk(vector<Segment> segments)
{
    int send_count=0;
    int i=0;
    while(send_count < cwnd && i<segments.size())
    {
        if(this->segments_status.at(i) == NOT_SENT)
            send_segment(segments[i]);
            send_count++;
        i++;
    }
}

// we initalize the segments_status to NOT SENT
vector<int> Sender::initialize_segments_status(int size) {
    vector<int> result;
    for (int i=0 ; i<size ; i++) {
        result.push_back(NOT_SENT);
    }
    return result;
}

//send segments to router
void Sender::send_segment(Segment segment)
{
    int src_port = this->sender_port;
    int dst_port = this->receiver_port;
    segment.set_ports(src_port, dst_port);

    char buffer[HEADER_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    segment.serialize(buffer);

    sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM,
            (const struct sockaddr *)&router_addr, sizeof(router_addr));
    segment.set_sent_time(time(NULL));
    cout << "Segment with seg_id " << segment.get_seg_id() << " sent" << endl;
}

//slices the file into segments
vector<Segment> Sender::slice_file(string file_location)
{
    vector<Segment> segments;
    char buffer[PAYLOAD_SIZE];
    memset(&buffer, 0, sizeof(buffer));

    int seg_id = 0;
    auto file_segment = Segment(file_location.c_str(), seg_id);
    seg_id++;

    segments.push_back(file_segment);

    std::ifstream infile(file_location);
    while (infile.read(buffer, PAYLOAD_SIZE))
    {
        segments.push_back(Segment(buffer, seg_id));
        seg_id++;

        memset(&buffer, 0, sizeof(buffer));
    }
    segments.push_back(Segment(buffer, seg_id));

    return segments;
}


void Sender::update_status(vector<bool> acks, vector<Segment> segments)
{

    //acks
    for(int i=0 ; i<acks.size() ; i++)
        this->segments_status.at(acks[i]) = RECEIVED; // if aks[i] = 1 means that we received ack so segments_status[i] -> RECEIVED
    for (int i=0 ; i<this->segments_status.size() ; i++) {
        if (this->segments_status.at(i) == SENT) {
            if(has_segment_expired(segments[i])) { // checks if segments[i] is has expired TIMEOUT
                this->segments_status.at(i) == NOT_SENT; // if segment[i] has expired put segments_status to NOT SENT so it sends again
                is_congest = true; //if timeout -> congestion occured
            }
        }
    }
}


bool Sender::should_wait()
{

    for (int i=0 ; i<this->segments_status.size() ; i++) {
        if (this->segments_status.at(i) == SENT) {
            return true;
        }
    }
    return false;
}

bool Sender::has_segment_expired(Segment segment)
{
  auto seg_time = segment.get_sent_time();

  bool has_expired = time(NULL) - seg_time > TIME_OUT_LENGTH;

  return has_expired;
}

vector<Segment> Sender::choose_segments_to_send(vector<Segment> segments)
{
    vector<Segment> chosen_segments;
    int i=0;
    while (chosen_segments.size() < cwnd) {
        if (this->segments_status.at(i) == NOT_SENT) {
            chosen_segments.push_back(segments[i]);
        }
        i++;
    }
    return chosen_segments;
}

void Sender::set_cwnd() {
    if (this->cwnd < this->ssthresh) {
        this->cwnd*= 2;
    } else {
        this->cwnd++;
    }
}

/*
    First we run sockets,
    then we slice the file into segments
    then we check if we need to send those segments
    we receive acks from receiver
    after receiving the acks we update the segments_status for congestion and acks
    if there is congestion we update the ssthresh and cwnd then we put is_congest to false

    then we check if acks are received if all the SENT acks are now RECEIVED, we send new segments, if not we wait till we RECEIVE them
    then we choose segments to send
    after that we send the chosen segments
    then we update cwnd

    NOTE: PLEASE CHECK THE LOGIC TO MAKE SURE THAT IT IS OKAY!!
*/
void Sender::start(string file_location)
{
  run_socket();

  auto segments = slice_file(file_location);
  this->segments_status = initialize_segments_status(segments.size());

  cout << "Segments array size " << segments.size() << endl;

  auto begin = std::chrono::steady_clock::now();

  while (still_sending())
  {
    // vector<int> acks = receive_acks(); //receiver is not ready
    vector<bool> acks;
    update_status(acks, segments);
    if(is_congest) {
        ssthresh = cwnd/2;
        cwnd = 1;
        is_congest = false;
        continue;
    }
    if(should_wait())
        continue;
    vector<Segment> segments_to_send;
    segments_to_send = choose_segments_to_send(segments);
    send_bulk(segments_to_send);
    set_cwnd();
  }
  auto end = std::chrono::steady_clock::now();
  cout << "file transmission time = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << endl;
}

int main(int argc, char *argv[])
{
  auto sender_port = stoi(argv[1]);
  auto receiver_port = stoi(argv[2]);
  auto router_port = stoi(argv[3]);
  auto file_location = string(argv[4]);
  Sender sender(sender_port, receiver_port, router_port);

  sender.start(file_location);
}
