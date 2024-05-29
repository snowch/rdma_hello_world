# Use a base image
FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

# Set working directory
WORKDIR /usr/src/rdma_example

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    libibverbs-dev \
    librdmacm-dev \
    rdmacm-utils \
    ibverbs-utils \
    iproute2 \
    iputils-ping \
    net-tools \
    software-properties-common \
    gdb \
    lsof

# Install Soft-RoCE
RUN add-apt-repository -y ppa:canonical-hwe-team/backport-iwlwifi && \
    apt-get update && \
    apt-get install -y rdma-core

# Copy the RDMA example code
COPY rdma_example/ ./

# Compile the example code
RUN make

# Entry point for the container
CMD ["bash"]
