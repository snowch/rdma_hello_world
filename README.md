#### RDMA Playground

This repository contains an example of setting up an RDMA environment, building and running an RDMA server and client, and sending a message between them. The Dockerfile and setup script automate the process, making it easier to reproduce the environment on different machines.

WARNING: This project is currently broken, see [Queue Pair is NULL](https://github.com/snowch/rdma_hello_world/issues/2)

##### Environment

- Mac OS M3 Apple Silicon with UTM
- Ubuntu 24.04 Linux VM running natively in UTM
- Soft-RoCE

#### Setup

- Clone this repository:
```git clone https://github.com/your-username/rdma_playground.git```
- Run the setup script:
```./setup.sh```
This script will:

- Install required packages for RDMA
- Configure Docker to use the NVIDIA container runtime for RDMA support
- Clone and build the RDMA core library
- Check for the rxe_cfg tool and build it if necessary
- Build the Docker image:
```./run.sh build```
- This will build a Docker image named rdma_lab containing the RDMA environment.

####  Running the Server and Client
Start the RDMA server:
```./run.sh server```
This will start the RDMA server in a Docker container.

####  Start the RDMA client:

```./run.sh client <server_ip_address>```
This will start the RDMA client in a Docker container and connect it to the server at the specified IP address.

####  Additional Notes

- The run.sh script includes options to configure the network interface and other parameters.
- The provided code uses the ibv_post_send function to send the message. This function is part of the InfiniBand Verbs API and requires appropriate permissions to access the RDMA hardware.
- The setup.sh script includes a step to configure Docker to use the NVIDIA container runtime for RDMA support. This is not strictly necessary for the provided example, but it may be useful for other RDMA applications.
- The rdma_client.c file includes a step to resolve the server's address and route. This is necessary because the server's IP address may not be known in advance.
- The run.sh script includes a step to build the Docker image. This is only necessary if the image has not already been built.

####  Contributing
Contributions are welcome! 

