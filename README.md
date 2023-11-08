# Socket over srsRAN "OAIC" version

This version of srsRAN is modified to enable server responses to client requests.

## Getting Started

Use the provided script to install the srsRAN OAIC version. After setting up the user equipment (UE), you'll be able to connect to the server by running the client.

### Prerequisites

Ensure 'OAIC' is installed on your system to proceed. After executing the script, configure the user equipment (UE), initiate traffic flow, and then launch the client.

### Installation Steps

cd oaic
```bash
git clone https://github.com/natanzi/socket-over-srsran
cd socket-over-srsran
cp srsran_with_socket.sh ../
cd ..
chmod +x srsran_with_socket.sh
./srsran_with_socket.sh
