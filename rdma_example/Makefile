CC = gcc
CFLAGS = -Wall -g

all: rdma_server rdma_client

rdma_server: rdma_server.o rdma_common.o
	$(CC) $(CFLAGS) -o rdma_server rdma_server.o rdma_common.o -libverbs -lrdmacm

rdma_client: rdma_client.o rdma_common.o
	$(CC) $(CFLAGS) -o rdma_client rdma_client.o rdma_common.o -libverbs -lrdmacm

rdma_server.o: rdma_server.c rdma_common.h
	$(CC) $(CFLAGS) -c rdma_server.c

rdma_client.o: rdma_client.c rdma_common.h
	$(CC) $(CFLAGS) -c rdma_client.c

rdma_common.o: rdma_common.c rdma_common.h
	$(CC) $(CFLAGS) -c rdma_common.c

clean:
	rm -f *.o rdma_server rdma_client
