# syntax=docker/dockerfile:1
# Build:
# docker build -t ghcr.io/nilfoundation/proof-market-toolchain:tag -t ghcr.io/nilfoundation/proof-market-toolchain:latest .
FROM ghcr.io/nilfoundation/proof-market-toolchain:base

WORKDIR /proof-market-toolchain

COPY . /proof-market-toolchain

RUN pip3 install --no-cache-dir -r /proof-market-toolchain/requirements.txt \
    && ./build.sh \
    && ln -s /proof-market-toolchain/build/bin/proof-generator/proof-generator /usr/bin/proof-generator
