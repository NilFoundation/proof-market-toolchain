# Installation

{% hint style="info" %}
Make sure you've performed the [environment setup](environment-setup.md) before
continuing with the installation.
{% endhint %}

The Proof Market toolchain is a collection of Python scripts for interacting with the Proof Market.
These scripts are required for all participants.

Clone the repository:

```bash
git clone --recurse-submodules git@github.com:NilFoundation/proof-market-toolchain.git
```

Based on the user's role, they must install additional binaries/scripts to interact with the market.

To interact with the proof market, you must also have Python setup and its dependencies.

We recommend setting up a virtual environment to interact with the proof market.

```
pip3 install --user virtualenv
python3 -m virtualenv venv
source venv/bin/activate
pip3 install -r requirements.txt
```



## Circuit developer

* Follow the installation guide for [zkLLVM](https://docs.nil.foundation/zkllvm/guides/installation).
  zkLLVM is used to compile circuits and generate binaries to be hosted on the Proof Market.
* Set up [authentication](../market/user-guides/sign-up.md) and follow the user guide
  for [circuit developers](../market/user-guides/circuit-developer.md).

## Proof requester

* Install [Lorem Ipsum CLI](https://github.com/NilFoundation/lorem-ipsum-cli).
  For proof validation, smart contracts or zkLLVM for local validation can be used.

* Set up [authentication](../market/user-guides/sign-up.md) and follow the user guide
  for [proof requesters](../market/user-guides/proof-requester.md)

## Proof producer

* As a proof producer, you need to compile the `proof-generator` binary.
  Currently, there are two options, a single-threaded (low performance) and a multithreaded one.

### Build

```bash
cd proof-market-toolchain && mkdir build && cd build
cmake -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=/usr/bin/clang-12 \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 \
  ..
# Single-threaded version
cmake --build . -t proof-generator
# Multi-threaded version
cmake --build . -t proof-generator-mt
```

* Set up [authentication](../market/user-guides/sign-up.md) and follow the user guide
  for [proof producers](../market/user-guides/proof-producer.md).