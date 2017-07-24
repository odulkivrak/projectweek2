#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdint.h>

#include "packets.h"

#include "sendselect.h"

 #if 0
#define MY_DEST_MAC0    0xff
#define MY_DEST_MAC1    0xff
#define MY_DEST_MAC2    0xff
#define MY_DEST_MAC3    0xff
#define MY_DEST_MAC4    0xff
#define MY_DEST_MAC5    0xff
#endif

#define ETHER_TYPE 0x1234
#define interface "enp0s3"
#define BUF_SIZ 1024

#define MY_NAME "odul"
#define MY_SURNAME "kivrak"

#define TARGET_NAME "durmus"
#define TARGET_SURNAME "orcan"

static void fill_query_bcast(struct query_bcast *q);

static void fill_query_ucast(struct query_ucast *q);

static void fill_hello_resp(struct hello_resp *q);

static void fill_chat(struct chat *q);

static void fill_chatack(struct chat_ack *q);

static void fill_exiting(struct exiting *q);

static void hex_print(void *pack, size_t len);

char packet_id;

/**
 * @brief
 *
 * @param[out] q
 */
static void fill_query_bcast(struct query_bcast *q)
{
    q->type = QUERY_BROADCAST;
    snprintf(q->name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
}

static void fill_query_ucast(struct query_ucast *q)
{
    q->type = QUERY_UNICAST;
    snprintf(q->name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
    snprintf(q->target_name, MAX_NAME_SIZE, "%s", TARGET_NAME);
    snprintf(q->target_surname, MAX_NAME_SIZE, "%s", TARGET_SURNAME);
}

static void fill_hello_resp(struct hello_resp *q)
{
    q->type = HELLO_RESPONSE;
    snprintf(q->responder_name, MAX_NAME_SIZE, "%s", TARGET_NAME);
    snprintf(q->responder_surname, MAX_NAME_SIZE, "%s", TARGET_SURNAME);
    snprintf(q->queryier_name, MAX_NAME_SIZE, "%s",  MY_NAME);
    snprintf(q->queryier_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);

}

static void fill_chat(struct chat *q)
{
    q->type = CHAT;
    snprintf(q->length, LENGTH, "%d", LENGTH);
    snprintf(q->packet_id, 1, "%c", packet_id);
    snprintf(q->message, MAX_NAME_SIZE, "%s","merhaba"); 
}

static void fill_chat_ack(struct chat_ack *q)
{
    q->type = CHAT_ACK;
    snprintf(q->packet_id, 1, "%c", packet_id);
}


static void fill_exiting(struct exiting *q)
{
    q->type = EXITING;
    snprintf(q->name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
}

/**
 * @brief generic print of struct in hexadecimal
 * format
 *
 * @param[in] pack
 * @param[in] len
 */
static void hex_print(void *pack, size_t len)
{
    int i = 0;
    char *arr = malloc(sizeof(char)*len);
    for (i = 0; i < len; i++) {
            arr[i] = ((uint8_t *) pack)[i];
    
}
}


/**
 * @brief
 *
 * @param[out] q
 */

int main(int argc, char *argv[])
{   

packet_id=0;

int sockfd;
    struct ifreq if_idx;
    struct ifreq if_mac;
    int tx_len = 0;
    char sendbuf[BUF_SIZ];
    struct ether_header *eh = (struct ether_header *) sendbuf;
    struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
    struct sockaddr_ll socket_address;
    char ifName[IFNAMSIZ];
    
    struct query_bcast query_bcast;
    struct query_ucast query_ucast;
    struct hello_resp hello_resp;
    struct chat chat;
    struct chat_ack chat_ack;
    struct exiting exiting;

    char *packet_bcast = malloc(sizeof(struct query_bcast));
    char *packet_ucast = malloc(sizeof(struct query_ucast));
    char *packet_hello_resp = malloc(sizeof(struct hello_resp));
    char *packet_chat = malloc(sizeof(struct chat));
    char *packet_chat_ack = malloc(sizeof(struct chat_ack));
    char *packet_exiting = malloc(sizeof(struct exiting));

    memset(&query_bcast, 0, sizeof(struct query_bcast));
    memset(&query_ucast, 0, sizeof(struct query_ucast));
    memset(&hello_resp, 0, sizeof(struct hello_resp));
    memset(&chat, 0, sizeof(struct chat));
    memset(&chat_ack, 0, sizeof(struct chat_ack));
    memset(&exiting, 0, sizeof(struct exiting));


    
    /* Get interface name */
    if (argc > 1)
      strcpy(ifName, argv[1]);
    else
      strcpy(ifName, interface);


char type=0x02;

switch(type){

    case(QUERY_BROADCAST):
    fill_query_bcast(&query_bcast);
    hex_print((void *) &query_bcast, sizeof(query_bcast));
    printf("type= %d, name= %s, surname= %s\n",query_bcast.type, query_bcast.name, query_bcast.surname);
    char MY_DEST_MAC_1[]={0xff,0xff,0xff,0xff,0xff,0xff};
    send_selection(MY_DEST_MAC_1[6], packet_bcast, QUERY_BROADCAST);
    break;

    case(QUERY_UNICAST):
    fill_query_ucast(&query_ucast);
    hex_print((void *) &query_ucast, sizeof(query_ucast));
    //packet_ucast =  &query_ucast;
    printf("type= %d, target name= %s, target surname= %s\n",query_ucast.type, query_ucast.target_name, query_ucast.target_surname);
    char MY_DEST_MAC_2[]={0x3c,0x07,0x71,0x56,0x89,0xa3};
    send_selection(MY_DEST_MAC_2, packet_ucast, QUERY_UNICAST);
    break;

    case(HELLO_RESPONSE):
    fill_hello_resp(&hello_resp);
    hex_print((void *) &hello_resp, sizeof(hello_resp));
    printf("type= %d, queryier name= %s, queryier surname= %s\n",hello_resp.type, hello_resp.queryier_name, hello_resp.queryier_surname);
    char MY_DEST_MAC_3[]={0xff,0xff,0xff,0xff,0xff,0xff};
    send_selection(MY_DEST_MAC_3, packet_hello_resp, HELLO_RESPONSE);
    break;

    case(CHAT):
    fill_chat(&chat);
    hex_print((void *) &chat, sizeof(chat));
    //printf("type= %d, length= %d, message_id= %d, message=%s\n",chat.type, chat.length, chat.message_id, chat.message);
    char MY_DEST_MAC_4[]={0x3c,0x07,0x71,0x56,0x89,0xa3};
    send_selection(MY_DEST_MAC_4, packet_chat, CHAT);
    break;

    case(CHAT_ACK):    
    fill_chat_ack(&chat_ack);
    hex_print((void *) &chat_ack, sizeof(chat_ack));
    printf("type= %d, packet_id= %d\n",chat_ack.type, chat_ack.packet_id);
    char MY_DEST_MAC_5[] ={0x08,0x00,0x27,0x01,0xcf,0x89};
    send_selection(MY_DEST_MAC_5, packet_chat_ack, CHAT_ACK);
    break;

    case(EXITING):
    fill_exiting(&exiting);
    hex_print((void *) &exiting, sizeof(exiting));
    //packet_exiting =  &query_exiting;
    printf("type= %d, name= %s, surname= %s\n",exiting.type, exiting.name, exiting.surname);
    break;

}

return 0;

}
