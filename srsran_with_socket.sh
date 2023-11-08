#!/bin/bash

# Check if the script is running in the 'oaic' folder
if [[ $(basename "$PWD") != "oaic" ]]; then
    echo "This script must be run from within the 'oaic' directory."
    exit 1
fi

# Update and install dependencies
apt-get update
apt-get install -y build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libtool autoconf libzmq3-dev

# Add UHD PPA and install UHD
add-apt-repository -y ppa:ettusresearch/uhd
apt-get update
apt-get install -y libuhd-dev libuhd4.5.0 uhd-host

# Clone the ASN1C repository and build it
git clone https://gitlab.eurecom.fr/oai/asn1c.git
cd asn1c
git checkout velichkov_s1ap_plus_option_group
autoreconf -iv
./configure
make -j$(nproc)
make install
ldconfig
cd ..

# Create the directory for the handover server and download files from the new repository
mkdir -p srsRAN-e2/srsenb/src/handover_server
cd srsRAN-e2/srsenb/src/handover_server
wget -O CMakeLists.txt https://raw.githubusercontent.com/natanzi/socket-over-srsran/main/handover_server/CMakeLists.txt
wget -O handover_server.cpp https://raw.githubusercontent.com/natanzi/socket-over-srsran/main/handover_server/handover_server.cpp
wget -O handover_server.h https://raw.githubusercontent.com/natanzi/socket-over-srsran/main/handover_server/handover_server.h
cd ..

# Download additional files from the new repository
wget -O CMakeLists.txt https://raw.githubusercontent.com/natanzi/socket-over-srsran/main/srsenb/src/CMakeLists.txt
wget -O main.cc https://raw.githubusercontent.com/natanzi/socket-over-srsran/main/srsenb/src/main.cc
cd ../..

# Create build directory and build srsRAN
mkdir build
export SRS=$(realpath .)
cd build
cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -DRIC_GENERATED_E2AP_BINDING_DIR=${SRS}/e2_bindings/E2AP-v01.01 \
          -DRIC_GENERATED_E2SM_KPM_BINDING_DIR=${SRS}/e2_bindings/E2SM-KPM \
          -DRIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR=${SRS}/e2_bindings/E2SM-GNB-NRT
make -j5
make install
ldconfig

# Install srsRAN configuration files as a service
srsran_install_configs.sh service
