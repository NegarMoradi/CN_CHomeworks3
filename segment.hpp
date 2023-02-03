#ifndef SEGMENT_HPP
#define SEGMENT_HPP

#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>

const int ACK_SIZE = 1;
const int SEGMENT_ID_SIZE = 2;
const int SRC_PORT_SIZE = 4;
const int DST_PORT_SIZE = 4;
const int PAYLOAD_SIZE = 1024;
const int SEGMENT_SIZE = 2048;

#define HEADER_SIZE ACK_SIZE + SEGMENT_ID_SIZE + SRC_PORT_SIZE + DST_PORT_SIZE + PAYLOAD_SIZE

using namespace std;

class Segment
{
private:
  int ack;
  int seg_id;
  int src_port;
  int dst_port;
  char payload[PAYLOAD_SIZE];

  time_t sent_time;


public:
  Segment(const char *_payload, int _seg_id);
  Segment();

  void set_ack(int ack_num);
  void set_ports(int _src_port, int _dst_port);


  int get_ack();
  int get_seg_id();
  int get_src_port();
  int get_dst_port();
  char* get_payload();

  char* serialize(char* buffer);
  void deserialize(char* buffer);

  void set_sent_time(time_t time);
  time_t get_sent_time();

  string slice(char *buffer, int start, int size);
  string make_fixed_size_str(const int input, const int length);
};
#endif
