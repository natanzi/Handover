# Socket over srsRAN "OAIC" version

This version of srsRAN is modified to enable server responses to client requests.

## Getting Started

Use the provided script to install the srsRAN OAIC version. After setting up the user equipment (UE), you'll be able to connect to the server by running the client.

### Prerequisites

Make sure you have 'OAIC' installed on your system before proceeding.
after running the script, add the ue and push traffic and after that, run client.

### Installation Steps

cd oaic
```bash
git clone https://github.com/natanzi/socket-over-srsran
cp -f socket-over-srsran/srsran_with_socket.sh ../
cd ..
chmod +x srsran_with_socket.sh
./srsran_with_socket.sh


