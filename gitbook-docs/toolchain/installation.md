---
description: Proof Market toolchain installation and building the toolchain from sources
---

# Toolchain installation

[Proof Market toolchain](https://github.com/NilFoundation/proof-market-toolchain)
is a collection of Python scripts for interacting with Proof Market.
These scripts are required for all participants.

Clone the repository:

```bash
git clone --recurse-submodules git@github.com:NilFoundation/proof-market-toolchain.git
```

Based on the user's role, they must install additional binaries/scripts to interact with the market.

To interact with Proof Market, you must also have Python setup and its dependencies.

We recommend setting up a virtual environment to interact with Proof Market.

```bash
pip3 install --user virtualenv
python3 -m virtualenv venv
source venv/bin/activate
pip3 install -r requirements.txt
```

## Circuit developer

* Follow the installation guide for [zkLLVM](https://docs.nil.foundation/zkllvm/guides/installation).
  zkLLVM is used to compile circuits and generate binaries to be hosted on Proof Market.
* Set up [authentication](../toolchain/sign-up.md) and follow the user guide
  for [zero-knowledge app developers](../developers/zk-app-developer.md).

## Proof requester

* Install [Lorem Ipsum CLI](https://github.com/NilFoundation/lorem-ipsum-cli).
  For proof validation, smart contracts or zkLLVM for local validation can be used.
* Set up [authentication](../toolchain/sign-up.md) and follow the user guide
  for [zero-knowledge app developers](../developers/zk-app-developer.md).

## Proof producer

As a proof producer, you need to compile the `proof-generator` binary.

### Build

```bash
cd proof-market-toolchain && mkdir build && cd build
cmake -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=/usr/bin/clang-12 \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 \
  ..
cmake --build . -t proof-generator
```

* Set up [authentication](../toolchain/sign-up.md) and follow the user guide
  for [proof producers](../producers/proof-producer.md).

# Building the toolchain from sources

You can learn more about the toolchain and how to build it via Docker or manually
in the [project's README](https://github.com/nilfoundation/proof-market-toolchain#readme).

