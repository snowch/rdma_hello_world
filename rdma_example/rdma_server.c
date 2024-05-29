#include "rdma_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 16

int main(int argc, char **argv) {
    struct rdma_cm_id *cm_id = NULL;
    struct rdma_cm_event *event = NULL;
    struct rdma_event_channel *ec = NULL;
    struct ibv_pd *pd = NULL;
    struct ibv_mr *mr = NULL;
    struct ibv_sge *sge = NULL;
    struct ibv_recv_wr *recv_wr = NULL;
    struct rdma_conn_param conn_param = {};

    char *msg = malloc(MSG_SIZE);
    if (!msg) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(msg, "Hello World!");

    ec = rdma_create_event_channel();
    if (!ec) {
        perror("rdma_create_event_channel");
        exit(EXIT_FAILURE);
    }

    if (rdma_create_id(ec, &cm_id, NULL, RDMA_PS_TCP)) {
        perror("rdma_create_id");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(20079);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (rdma_bind_addr(cm_id, (struct sockaddr *)&addr)) {
        perror("rdma_bind_addr");
        exit(EXIT_FAILURE);
    }

    if (rdma_listen(cm_id, 1)) {
        perror("rdma_listen");
        exit(EXIT_FAILURE);
    }

    while (rdma_get_cm_event(ec, &event) == 0) {
        if (event->event == RDMA_CM_EVENT_CONNECT_REQUEST) {
            printf("Connection requested.\n");

            if (!event->id) {
                printf("Event ID is NULL\n");
                exit(EXIT_FAILURE);
            }
            printf("Event ID: %p\n", event->id);

            pd = rc_get_pd(event->id);
            if (!pd) {
                perror("rc_get_pd");
                exit(EXIT_FAILURE);
            }
            printf("PD: %p\n", pd);

            mr = rc_get_reg_mr(pd, msg, MSG_SIZE);
            if (!mr) {
                perror("rc_get_reg_mr");
                exit(EXIT_FAILURE);
            }
            printf("MR: %p\n", mr);

            sge = rc_get_sge(mr);
            if (!sge) {
                perror("rc_get_sge");
                exit(EXIT_FAILURE);
            }
            printf("SGE: %p\n", sge);

            recv_wr = rc_get_recv_wr(sge);
            if (!recv_wr) {
                perror("rc_get_recv_wr");
                exit(EXIT_FAILURE);
            }
            printf("Recv WR: %p\n", recv_wr); 
            printf("Queue Pair Type: %p\n",event->id->qp_type);
            printf("Port Number: %p\n",event->id->port_num);

            if (!event->id->qp) {
                printf("Queue pair is NULL\n");
                exit(EXIT_FAILURE);
            }
            printf("Queue pair: %p\n", event->id->qp);

            if (ibv_post_recv(event->id->qp, recv_wr, NULL)) {
                perror("ibv_post_recv");
                exit(EXIT_FAILURE);
            }
            printf("Recv WR posted.\n");

            conn_param.responder_resources = 1;
            conn_param.initiator_depth = 1;
            conn_param.retry_count = 7;

            printf("Accepting connection...\n");
            if (rdma_accept(event->id, &conn_param)) {
                perror("rdma_accept");
                exit(EXIT_FAILURE);
            }
            printf("Connection accepted.\n");

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
