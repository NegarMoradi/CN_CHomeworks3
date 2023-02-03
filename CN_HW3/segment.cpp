#include "segment.hpp"
#include <iostream>

Segment::Segment(const char *_payload, int _seg_id)
{
  this->ack = 0;
  this->seg_id = _seg_id;
  strncpy(this->payload, _payload, PAYLOAD_SIZE);
  this->payload[PAYLOAD_SIZE] = '\0';
}

Segment::Segment()
{
  this->payload[0] = '\0';
  this->ack = 0;
}

void Segment::set_ports(int _src_port, int _dst_port)
{
  this->src_port = _src_port;
  this->dst_port = _dst_port;
}

void Segment::set_ack(int _ack)
{
  this->ack = _ack;
}

void Segment::set_sent_time(time_t time)
{
  this->sent_time = time;
}

time_t Segment::get_sent_time()
{
  return this->sent_time;
}

int Segment::get_seg_id()
{
  return this->seg_id;
}

int Segment::get_ack()
{
  return this->ack;
}

char* Segment::get_payload()
{
  return this->payload;
}

int Segment::get_dst_port()
{
  return this->dst_port;
}

int Segment::get_src_port()
{
  return this->src_port;
}

char* Segment::serialize(char* buffer)
{

    auto ack_str = make_fixed_size_str(this->ack, ACK_SIZE);
    auto seg_id_str = make_fixed_size_str(this->seg_id, SEGMENT_ID_SIZE);
    auto src_port_str = make_fixed_size_str(this->src_port, SRC_PORT_SIZE);
    auto dst_port_str = make_fixed_size_str(this->dst_port, DST_PORT_SIZE);

    strcpy(buffer, ack_str.c_str());
    strcpy(buffer + strlen(buffer), seg_id_str.c_str());
    strcpy(buffer + strlen(buffer), src_port_str.c_str());
    strcpy(buffer + strlen(buffer), dst_port_str.c_str());

    strcpy(buffer + strlen(buffer), this->payload);
    buffer[strlen(buffer)] = '\0';

    return buffer;
}

void Segment::deserialize(char *buffer)
{
  int index = 0;
  this->ack = stoi(slice(buffer, index, ACK_SIZE));
  index += ACK_SIZE;

  this->seg_id = stoi(slice(buffer, index, SEGMENT_ID_SIZE));
  index += SEGMENT_ID_SIZE;

  this->src_port = stoi(slice(buffer, index, SRC_PORT_SIZE));
  index += SRC_PORT_SIZE;

  this->dst_port = stoi(slice(buffer, index, DST_PORT_SIZE));
  index += DST_PORT_SIZE;

  memset(this->payload, 0, PAYLOAD_SIZE);
  strncpy(this->payload, buffer + index, strlen(buffer) - index);
  this->payload[strlen(buffer) - index] = '\0';
}

string Segment::slice(char *buffer, int start, int size)
{
  return string(buffer).substr(start, size);
}

string Segment::make_fixed_size_str(const int input, const int length)
{
  ostringstream ss;

  if (input < 0)
    ss << '-';

  ss << setfill('0') << setw(length) << (input < 0 ? -input : input);

  return ss.str();
}
