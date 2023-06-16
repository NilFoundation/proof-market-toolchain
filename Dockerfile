# syntax=docker/dockerfile:1

FROM ghcr.io/nilfoundation/proof-market-toolchain:base

WORKDIR /proof-market-toolchain

COPY . /proof-market-toolchain

RUN ./build.sh \
      && ln -s /proof-market-toolchain/build/bin/proof-generator/proof-generator /usr/bin/proof-generator
