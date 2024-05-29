#!/bin/bash 

set -e

# Check if rdma_rxe module is loaded
if ! lsmod | grep -q "rdma_rxe"; then
    echo "Loading rdma_rxe module..."
    sudo modprobe rdma_rxe
    if [ $? -ne 0 ]; then
        echo "Error: Failed to load rdma_rxe module."
        exit 1
    fi
fi

# Check if ibstat command is available
if ! command -v ibstat &> /dev/null; then
    echo "Error: ibstat command is not available. Please ensure infiniband-diags package is installed correctly."
    exit 1
fi

# Check if IB device is detected
if ! ibstat &> /dev/null; then
    echo "Error: IB device can't be found."
    exit 1
fi

# Check if rxe0 interface exists
if ip link show dev rxe0 &> /dev/null; then
    echo "rxe0 interface already exists."
else
    # Add rxe0 interface
    sudo ib link add name rxe0 type rxe
    if [ $? -eq 0 ]; then
        echo "rxe0 interface created successfully."
    else
        echo "Error: Failed to create rxe0 interface."
        exit 1
    fi
fi

# Set rxe0 interface up
sudo ip link set dev rxe0 up
if [ $? -eq 0 ]; then
    echo "rxe0 interface set up successfully."
else
    echo "Error: Failed to set up rxe0 interface."
    exit 1
fi

# Check if rxe0 interface is UP
if ! ip link show dev rxe0 | grep -q "UP"; then
    echo "Error: rxe0 interface is not UP."
    exit 1
fi

# Display information about rxe0 interface
echo "Information about rxe0 interface:"
sudo ip -d link show dev rxe0

echo "Test completed successfully."
