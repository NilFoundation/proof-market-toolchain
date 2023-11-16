#!/bin/bash

# Define the build directory and the repository URL
BUILD_DIR="proof-producer-build"
REPO_URL="https://github.com/NilFoundation/proof-producer"

sudo wget https://github.com/NilFoundation/zkLLVM/releases/download/v0.1.7/zkllvm_0.1.7_amd64.deb
dpkg -i zkllvm_0.1.7_amd64.deb
# Install necessary dependencies (e.g., git, cmake, and build-essential)
sudo apt update
sudo apt install -y git cmake build-essential

# Clone the repository
git clone ${REPO_URL}

# Create a build directory and navigate into it
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Compile the project using CMake
cmake ../proof-producer
make

# Install the compiled project
sudo make install
