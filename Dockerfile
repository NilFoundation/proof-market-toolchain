# syntax=docker/dockerfile:1

FROM ubuntu:22.04
RUN DEBIAN_FRONTEND=noninteractive \
    set -xe \
    && apt-get update \
    && apt-get -y --no-install-recommends --no-install-suggests install \
        autoconf \
        automake \
        build-essential \
        clang-12 \
        cmake \
        git \
        gnutls-dev \
        libc-ares-dev \
        libfmt-dev \
        libhwloc-dev \
        liblz4-dev \
        libprotobuf-dev \
        libsctp-dev \
        libssl-dev \
        libyaml-cpp-dev \
        pkg-config \
        ragel \
        systemtap-sdt-dev \
        wget \
        xfslibs-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN set -xe \
    && wget -q --no-check-certificate \
      https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz \
    && tar -xvf boost_1_76_0.tar.gz \
    && rm boost_1_76_0.tar.gz

WORKDIR /tmp/boost_1_76_0
RUN set -xe \
    && sh ./bootstrap.sh \
    && ./b2 \
    && ./b2 install

WORKDIR /proof-market-toolchain
