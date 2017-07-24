
enum types{
   QUERY_BROADCAST ,
   QUERY_UNICAST ,
   HELLO_RESPONSE,
   CHAT,
   CHAT_ACK,
   EXITING
} en_pack_type;

struct query_bcast{
  char type;
  char requester_name[10];
  char requester_surname[10];
};

struct query_ucast{
  char type;
  char requester_name[10];
  char requester_surname[10];
  char target_name[10];
  char target_surname[10];

};

struct hello_resp{
  char type;
  char responder_name[10];
  char responder_surname[10];
  char queryier_name[10];
  char queryier_surname[10];

};

struct chat{
  char type;
  char length[2];
  char packet_id;
  char message[10];

};

struct chat_ack{
  char type;
  char packet_id;
};

struct exiting{
  char type;
  char *name;
  char *surname;
};

static void fill_query_bcast(struct query_bcast *q);

static void fill_query_ucast(struct query_ucast *q, char *target_name, char *target_surname);

static char *hex_print(void *pack, size_t len);

static void decode_ucast();

static void decode_bcast();

//int struct_to_hex(char *input, int output);
//char hex_to_struct(int input, char *output);
