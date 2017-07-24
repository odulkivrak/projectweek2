#ifndef _PACKETS_H
#define _PACKETS_H

#define MAX_NAME_SIZE 10
#define LENGTH 2
enum {
   QUERY_BROADCAST,
   QUERY_UNICAST,
   HELLO_RESPONSE,
   CHAT,
   CHAT_ACK,
   EXITING,
} EN_PACKET;

struct query_bcast {
    uint8_t type;
    char name[MAX_NAME_SIZE];
    char surname[MAX_NAME_SIZE];
} __attribute__((packed));

struct query_ucast {
    uint8_t type;
    char name[MAX_NAME_SIZE];
    char surname[MAX_NAME_SIZE];
    char target_name[MAX_NAME_SIZE];
    char target_surname[MAX_NAME_SIZE]; 
}  __attribute__((packed));


struct hello_resp{
  char type;
  char responder_name[MAX_NAME_SIZE];
  char responder_surname[MAX_NAME_SIZE];
  char queryier_name[MAX_NAME_SIZE];
  char queryier_surname[MAX_NAME_SIZE];

} __attribute__((packed));

struct chat{
  char type;
  char length[LENGTH];
  char packet_id;
  char message[MAX_NAME_SIZE];

} __attribute__((packed));

struct chat_ack{
  char type;
  char packet_id;
} __attribute__((packed));

struct exiting{
  char type;
  char *name;
  char *surname;
} __attribute__((packed));

#endif
