#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "neweth.h"
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define DEST_MAC0 0x08
#define DEST_MAC1 0x00
#define DEST_MAC2 0x27
#define DEST_MAC3 0x01
#define DEST_MAC4 0xcf
#define DEST_MAC5 0x89

#define ETHER_TYPE  0x1234

#define DEFAULT_IF  "enp0s3"
#define BUF_SIZ   1024
#define HEAD_SIZE   14
#define MY_NAME "odul"
#define MY_SURNAME "kivrak"
#define TARGET_NAME "durmus"
#define TARGET_SURNAME "orcan"
#define MAX_NAME_SIZE 10
#define MAC_SIZE 6


static uint8_t bcast[sizeof(struct query_bcast)];
static uint8_t ucast[sizeof(struct query_ucast)];
static uint8_t smac[MAC_SIZE];
static uint8_t dmac[MAC_SIZE];
static uint8_t type;




char *hex_to_string(char *string, size_t length)
{

    char *pos = string;
    char *val = malloc(sizeof(char)*length);
    size_t count = 0;
    for(count = 0; count < length; count++) {
        sscanf(pos, "%2hhx", &val[count]);
        pos += 2;
        //printf("%c\n", val[count]);
    }
    return val;
}


int main(int argc, char *argv[]){
    struct query_bcast query_bcast;
    struct query_ucast query_ucast;

    memset(&query_bcast, 0, sizeof(struct query_bcast));
    memset(&query_ucast, 0, sizeof(struct query_ucast));

    fill_query_bcast(&query_bcast);
    //hex_print((void *) &query_bcast, sizeof(struct query_bcast));
    //printf("\n");
    //fill_query_ucast(&query_ucast);
    //hex_print((void *) &query_ucast, sizeof(struct query_ucast));
    //printf("\n");



  /*  switch (type) {
      case QUERY_BROADCAST:
      case QUERY_UNICAST:
      case HELLO_RESPONSE:
      case CHAT:
      case CHAT_ACK:
      default: return 0;
    }*/

    char sender[INET6_ADDRSTRLEN];
    int sockfd, ret, i;
    int sockopt;
    ssize_t numbytes;
    struct ifreq ifopts;  /* set promiscuous mode: All data received and read */
    struct ifreq if_ip; /* get ip addr */
    struct sockaddr_storage their_addr;
    uint8_t buf[BUF_SIZ];
    uint8_t data[BUF_SIZ-HEAD_SIZE];
    char ifName[IFNAMSIZ];
    strcpy(ifName, DEFAULT_IF);
    /* Get interface name */
    /*if (argc > 1)
      type = atoi(argv[1]);
      switch (type) {
          case QUERY_BROADCAST:
          case QUERY_UNICAST:
            strcpy(target_name, argv[2]);
            strcpy(target_surname, argv[3]);
          case HELLO_RESPONSE:
            strcpy(target_name, argv[2]);
            strcpy(target_surname, argv[3]);
          case CHAT:
          case CHAT_ACK:
          default: print;
        }
      type = atoi(argv[1]);
      strcpy(target_name, argv[2]);
      strcpy(target_surname, argv[3]);*/

    FILE *fp;
    fp = fopen("macs.txt", "w+");
    fclose(fp);
    /* Header structures */
    struct ether_header *eh = (struct ether_header *) buf;
    struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
    struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

    memset(&if_ip, 0, sizeof(struct ifreq));

    /* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
      perror("listener: socket");
      return -1;
    }

    /* Set interface to promiscuous mode - do we need to do this every time? */
    strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
    ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
    ifopts.ifr_flags |= IFF_PROMISC;
    ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
    /* Allow the socket to be reused - incase connection is closed prematurely */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
      perror("setsockopt");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    /* Bind to device */
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)  {
      perror("SO_BINDTODEVICE");
      close(sockfd);
      exit(EXIT_FAILURE);
    }

  repeat: //printf("listener: Waiting to recvfrom...\n");
    numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
    //printf("listener: got packet %lu bytes\n", numbytes);

    /* Check the packet is for me */
    /*if (eh->ether_dhost[0] == DEST_MAC0 &&
        eh->ether_dhost[1] == DEST_MAC1 &&
        eh->ether_dhost[2] == DEST_MAC2 &&
        eh->ether_dhost[3] == DEST_MAC3 &&
        eh->ether_dhost[4] == DEST_MAC4 &&
        eh->ether_dhost[5] == DEST_MAC5) {
      //printf("Correct destination MAC address\n");
    } else {
      printf("Wrong destination MAC: %x:%x:%x:%x:%x:%x\n",
              eh->ether_dhost[0],
              eh->ether_dhost[1],
              eh->ether_dhost[2],
              eh->ether_dhost[3],
              eh->ether_dhost[4],
              eh->ether_dhost[5]);
      ret = -1;
      goto done;
    }*/

    /* Get source IP */
    ((struct sockaddr_in *)&their_addr)->sin_addr.s_addr = iph->saddr;
    inet_ntop(AF_INET, &((struct sockaddr_in*)&their_addr)->sin_addr, sender, sizeof sender);

    /* Look up my device IP addr if possible */
    strncpy(if_ip.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0) { /* if we can't check then don't */
      /*printf("Source IP: %s\n My IP: %s\n", sender,
          inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));*/
      /* ignore if I sent it */
      if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)  {
        printf("but I sent it :(\n");
        ret = -1;
        goto done;
      }
    }

    /* UDP payload length */
    ret = ntohs(udph->len) - sizeof(struct udphdr);

    /* Print packet */
    printf("-------------------------------------\n");
    printf("Dest MAC: ");

    for (i=0; i<numbytes; i++) {
      if(i < 6){
        printf("%02x:", buf[i]);
        smac[i] = buf[i];
      }
      else if(i==6)
        printf("\nSrc MAC: " );
      if(i >= 6 && i < HEAD_SIZE-2){
        printf("%02x:", buf[i]);
        dmac[i] = buf[i-6];
      }

    }
    printf("\n");

    /*for (i = HEAD_SIZE; i<numbytes; i++) {
      bcast[i-HEAD_SIZE] = buf[i];

    }*/
    for (i = HEAD_SIZE; i<numbytes; i++) {
      ucast[i-HEAD_SIZE] = buf[i];

    }
    printf("\n");
    //decode_bcast();
    decode_ucast();

    printf("\n");


    /*switch (type) {
      case QUERY_BROADCAST:
      case QUERY_UNICAST:
      case HELLO_RESPONSE:
      case CHAT:
      case CHAT_ACK:
      default: return 0;
    }*/
  done: goto repeat;

    close(sockfd);
    return ret;
  }

  static void fill_query_bcast(struct query_bcast *q)
  {
      q->type = QUERY_BROADCAST;
      snprintf(q->requester_name, MAX_NAME_SIZE, "%s", MY_NAME);
      snprintf(q->requester_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
  }

  /**
   * @brief
   *
   * @param[out] q
   */
  static void fill_query_ucast(struct query_ucast *q, char *target_name, char *target_surname)
  {
      q->type = QUERY_UNICAST;
      snprintf(q->requester_name, MAX_NAME_SIZE, "%s", MY_NAME);
      snprintf(q->requester_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
      snprintf(q->target_name, MAX_NAME_SIZE, "%s", target_name);
      snprintf(q->target_surname, MAX_NAME_SIZE, "%s", target_surname);
  }

  static void fill_hello_resp(struct hello_resp *q, char *responder_name)
  {
      q->type = HELLO_RESPONSE;
      snprintf(q->responder_name, MAX_NAME_SIZE, "%s", MY_NAME);
      snprintf(q->responder_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
      snprintf(q->queryier_name, MAX_NAME_SIZE, "%s", TARGET_NAME);
      snprintf(q->queryier_surname, MAX_NAME_SIZE, "%s", TARGET_SURNAME);
  }

  static void fill_chat(struct chat *q)
  {
      q->type = HELLO_RESPONSE;
      snprintf(q->length, MAX_NAME_SIZE, "%s", MY_NAME);
      //q->packet_id
      //snprintf(q->queryier_name, MAX_NAME_SIZE, "%s", TARGET_NAME);
      //snprintf(q->queryier_surname, MAX_NAME_SIZE, "%s", TARGET_SURNAME);
  }

  /**
   * @brief generic print of struct in hexadecimal
   * format
   *
   * @param[in] pack
   * @param[in] len
   */
/*  static void hex_print(void *pack, size_t len)
  {
      int i = 0;
      for (i = 0; i < len; i++) {
          fprintf(stdout, "%02x ", ((uint8_t *) pack)[i]);
      }
      fprintf(stdout, "\n");
  }*/

  static void decode_bcast()
  {
    struct query_bcast *q;
    q = (struct query_bcast*) bcast;

      fprintf(stdout, "q->type: %d\n", q->type);

    fprintf(stdout, "q->requester_name: %s\n", q->requester_name);
    fprintf(stdout, "q->requester_surname: %s\n", q->requester_surname);
  }

  static void decode_ucast()
  {
    struct query_ucast *q;
    q = (struct query_ucast*) ucast;

    fprintf(stdout, "q->type: %d\n", q->type);

    fprintf(stdout, "q->requester_name: %s\n", q->requester_name);
    fprintf(stdout, "q->requester_surname: %s\n", q->requester_surname);

    fprintf(stdout, "q->target_name: %s\n", q->target_name);
    fprintf(stdout, "q->target_surname: %s\n", q->target_surname);
    printf("--------------------------------------\n");
}