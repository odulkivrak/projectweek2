
#define _SENDSELECT_H

static void fill_query_bcast(struct query_bcast *q);

static void fill_query_ucast(struct query_ucast *q);

static void fill_hello_resp(struct hello_resp *q);

static void fill_chat(struct chat *q);

static void fill_chat_ack(struct chat_ack *q);

static void fill_exiting(struct exiting *q);

static void hex_print(void *pack, size_t len);

void send_selection(char MY_DEST_MAC[6], char* packet,char type);


//int struct_to_hex(char *input, int output);
//char hex_to_struct(int input, char *output);

