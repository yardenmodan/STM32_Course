#ifndef INC_SERVER_H
#define INC_SERVER_H
#include "udp.h"

void udp_receive_callback(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port);
err_t send_packet(struct udp_pcb* pcb, const void* payload, u16_t payload_len, const ip_addr_t* ipaddr, u16_t port);
void udp_disconnect(struct udp_pcb*);   // Disconnect the UDP server
err_t udp_send(struct udp_pcb *, struct pbuf *);  // Send a UDP packet
err_t udp_connect(struct udp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port);  // Connect to a remote host
struct udp_pcb *udp_new(void);           // Allocate a new udp_pcb structure
err_t udp_bind(struct udp_pcb *, const ip_addr_t *, u16_t);     // Bind the UDP server to an IP address and port number
void udp_recv(struct udp_pcb *pcb, udp_recv_fn recv, void *recv_arg);              // Receive a UDP packet
void udp_remove(struct udp_pcb *);
err_t udp_sendto(struct udp_pcb *, struct pbuf *, const ip_addr_t *, u16_t);  // Send a UDP packet to a specific IP address and port number
void udpServer_init(void);


#define MAX_BUF_LEN 264
#define SERVER_PORT 5000


#endif
