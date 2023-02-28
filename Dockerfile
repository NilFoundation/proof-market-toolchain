# syntax=docker/dockerfile:1
   
FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt -y install wget ragel libsctp-dev build-essential libssl-dev cmake clang-12 git autoconf libc-ares-dev libfmt-dev gnutls-dev liblz4-dev libprotobuf-dev libyaml-cpp-dev libhwloc-dev pkg-config xfslibs-dev systemtap-sdt-dev

WORKDIR /tmp

RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz
RUN tar -xvf boost_1_76_0.tar.gz
RUN cd boost_1_76_0
WORKDIR /tmp/boost_1_76_0
RUN sh ./bootstrap.sh
RUN ./b2
RUN ./b2 install

WORKDIR /proof-market-toolchain




