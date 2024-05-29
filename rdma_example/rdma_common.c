#include "rdma_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rc_die(const char *reason) {
    fprintf(stderr, "%s Error code: %d\n", reason, errno); // Print the reason and error code
    exit(EXIT_FAILURE);
}

struct ibv_pd *rc_get_pd(struct rdma_cm_id *id) {
    if (id->pd)
        return id->pd;
    else {
        struct ibv_pd *pd = ibv_alloc_pd(id->verbs);
        if (!pd)
            rc_die("ibv_alloc_pd failed");
        return pd;
    }
}

struct ibv_comp_channel *rc_get_comp_channel(struct rdma_cm_id *id) {
    return id->send_cq ? id->send_cq->channel : ibv_create_comp_channel(id->verbs);
}

struct ibv_cq *rc_get_cq(struct rdma_cm_id *id) {
    return id->send_cq ? id->send_cq : ibv_create_cq(id->verbs, 10, NULL, NULL, 0);
}

struct ibv_mr *rc_get_reg_mr(struct ibv_pd *pd, void *addr, size_t length) {
    return ibv_reg_mr(pd, addr, length, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
}

struct ibv_sge *rc_get_sge(struct ibv_mr *mr) {
    struct ibv_sge *sge = (struct ibv_sge *)malloc(sizeof(struct ibv_sge));
    sge->addr = (uintptr_t)mr->addr;
    sge->length = mr->length;
    sge->lkey = mr->lkey;
    return sge;
}

struct ibv_send_wr *rc_get_send_wr(struct ibv_sge *sge) {
    struct ibv_send_wr *send_wr = (struct ibv_send_wr *)malloc(sizeof(struct ibv_send_wr));
    memset(send_wr, 0, sizeof(struct ibv_send_wr));
    send_wr->sg_list = sge;
    send_wr->num_sge = 1;
    send_wr->opcode = IBV_WR_SEND;
    return send_wr;
}

struct ibv_recv_wr *rc_get_recv_wr(struct ibv_sge *sge) {
    struct ibv_recv_wr *recv_wr = (struct ibv_recv_wr *)malloc(sizeof(struct ibv_recv_wr));
    memset(recv_wr, 0, sizeof(struct ibv_recv_wr));
    recv_wr->sg_list = sge;
    recv_wr->num_sge = 1;
    return recv_wr;
}