# syntax=docker/dockerfile:1
FROM ubuntu:22.04
RUN DEBIAN_FRONTEND=noninteractive \
    set -xe \
    && apt-get update \
    && apt-get -y --no-install-recommends --no-install-suggests install \
        autoconf \
        automake \
        build-essential \
        cmake \
        mc \
        nano \
        git \
        gnutls-dev \
        libc-ares-dev \
        libfmt-dev \
        libhwloc-dev \
        liblz4-dev \
        libprotobuf-dev \
        libsctp-dev \
        systemctl \
        libssl-dev \
        libyaml-cpp-dev \
        pkg-config \
        ragel \
        systemtap-sdt-dev \
        wget \
        xfslibs-dev \
        python3 \
        python3-pip \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN pip install numpy 
RUN pip install requests 

WORKDIR /

ENTRYPOINT [ "python3", "/proof-market-toolchain/scripts/request_pusher/request_loop.py" ]