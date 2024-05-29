#!/bin/bash

interface=rxe0

# Function to build the Docker image
build_docker_image() {
    docker build -t rdma_lab .
}

# Function to run RDMA server
run_rdma_server() {
    #docker run --rm --network=host --privileged -e INTERFACE=$interface rdma_lab /usr/src/rdma_example/rdma_server $interface &
    docker run --rm --network=host --privileged -e INTERFACE=$interface rdma_lab sleep infinity & 
    # rdma_lab /usr/src/rdma_example/rdma_server $interface &
}

# Function to run RDMA client
run_rdma_client() {
    local ip_address=$1
    #docker run --rm --network=host --privileged -e INTERFACE=$interface rdma_lab /usr/src/rdma_example/rdma_client
    set -x
    docker run --network=host --privileged -e INTERFACE=$interface rdma_lab /usr/src/rdma_example/rdma_client $ip_address
}

# Check if at least one argument is provided
if [ $# -lt 1 ]; then
    echo "Error: Please specify whether to run as 'client' or 'server'."
    exit 1
fi

# Validate the first argument (mode)
mode=$1
if [ "$mode" != "client" ] && [ "$mode" != "server" ]; then
    echo "Error: Invalid mode. Please specify either 'client' or 'server'."
    exit 1
fi

# If mode is 'client', check for the second argument (IP address)
if [ "$mode" = "client" ] && [ $# -lt 2 ]; then
    echo "Error: Please provide an IP address for client mode."
    exit 1
fi

# Get the IP address for client mode
if [ "$mode" = "client" ]; then
    ip_address=$2
fi

echo "Network interface $interface is valid. Proceeding with the script..."

echo "Building Docker image..."
build_docker_image

if [ "$mode" = "server" ]; then
    echo "Starting RDMA server..."
    run_rdma_server
    sleep 2
elif [ "$mode" = "client" ]; then
    echo "Starting RDMA client..."
    run_rdma_client $ip_address
fi
