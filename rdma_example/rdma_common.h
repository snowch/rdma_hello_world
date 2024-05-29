#ifndef RDMA_COMMON_H
#define RDMA_COMMON_H

#include <infiniband/verbs.h>
#include <rdma/rdma_cma.h>

#define TEST_NZ(x) do { if ((x)) rc_die("error: " #x " failed (returned non-zero)."); } while (0)
#define TEST_Z(x)  do { if (!(x)) rc_die("error: " #x " failed (returned zero/null)."); } while (0)

void rc_die(const char *reason);
struct ibv_pd *rc_get_pd(struct rdma_cm_id *id);
struct ibv_comp_channel *rc_get_comp_channel(struct rdma_cm_id *id);
struct ibv_cq *rc_get_cq(struct rdma_cm_id *id);
struct ibv_mr *rc_get_reg_mr(struct ibv_pd *pd, void *addr, size_t length);
struct ibv_sge *rc_get_sge(struct ibv_mr *mr);
struct ibv_send_wr *rc_get_send_wr(struct ibv_sge *sge);
struct ibv_recv_wr *rc_get_recv_wr(struct ibv_sge *sge);

#endif