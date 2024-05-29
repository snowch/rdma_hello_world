#include "rdma_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // For inet_addr

#define MSG_SIZE 16

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *ip_address = argv[1];

    struct rdma_cm_id *cm_id = NULL;
    struct rdma_cm_event *event = NULL;
    struct rdma_event_channel *ec = NULL;
    struct ibv_pd *pd = NULL;
    struct ibv_mr *mr = NULL;
    struct ibv_sge *sge = NULL;
    struct ibv_send_wr *send_wr = NULL;
    struct rdma_conn_param conn_param = {};

    char *msg = malloc(MSG_SIZE);
    if (!msg) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(msg, "Hello World!");

    printf("Creating event channel...\n");
    ec = rdma_create_event_channel();
    if (!ec) {
        perror("rdma_create_event_channel");
        exit(EXIT_FAILURE);
    }

    printf("Creating RDMA identifier...\n");
    if (rdma_create_id(ec, &cm_id, NULL, RDMA_PS_TCP)) {
        perror("rdma_create_id");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(20079);
    addr.sin_addr.s_addr = inet_addr(ip_address);

    printf("Resolving address...\n");
    if (rdma_resolve_addr(cm_id, NULL, (struct sockaddr *)&addr, 2000)) {
        perror("rdma_resolve_addr");
        exit(EXIT_FAILURE);
    }
    
    while (rdma_get_cm_event(ec, &event) == 0) {
        if (event->event == RDMA_CM_EVENT_ADDR_RESOLVED) {
            printf("Address resolved.\n");
            pd = rc_get_pd(event->id);
            if (!pd) {
                perror("rc_get_pd");
                exit(EXIT_FAILURE);
            }

            mr = rc_get_reg_mr(pd, msg, MSG_SIZE);
            if (!mr) {
                perror("rc_get_reg_mr");
                exit(EXIT_FAILURE);
            }

            sge = rc_get_sge(mr);
            if (!sge) {
                perror("rc_get_sge");
                exit(EXIT_FAILURE);
            }

            send_wr = rc_get_send_wr(sge);
            if (!send_wr) {
                perror("rc_get_send_wr");
                exit(EXIT_FAILURE);
            }

            printf("Registering memory region.\n");

            if (rdma_resolve_route(event->id, 2000)) {
                perror("rdma_resolve_route");
                exit(EXIT_FAILURE);
            }

            rdma_ack_cm_event(event);
        } else if (event->event == RDMA_CM_EVENT_ROUTE_RESOLVED) {
            printf("Route resolved.\n");
            conn_param.initiator_depth = 1;
            conn_param.retry_count = 7;

            printf("Accepting connection...\n");
            if (rdma_connect(event->id, &conn_param)) {
                perror("rdma_connect");
                exit(EXIT_FAILURE);
            }
            printf("Connection established.\n");

            if (!event->id->qp) {
                printf("Queue pair is NULL\n");
                exit(EXIT_FAILURE);
            }
            printf("Queue pair: %p\n", event->id->qp);

            if (ibv_post_send(event->id->qp, send_wr, NULL)) {
                perror("ibv_post_send");
                exit(EXIT_FAILURE);
            }

            printf("Message sent: %s\n", msg);
            rdma_ack_cm_event(event);
        } else if (event->event == RDMA_CM_EVENT_ESTABLISHED) {
            printf("Connection established.\n");
            rdma_ack_cm_event(event);
        } else if (event->event == RDMA_CM_EVENT_DISCONNECTED) {
            printf("Connection closed.\n");
            rdma_ack_cm_event(event);
            break;
        }
    }

    // Cleanup resources
    if (msg) free(msg);
    if (cm_id) rdma_destroy_id(cm_id);
    if (ec) rdma_destroy_event_channel(ec);

    return 0;
}
