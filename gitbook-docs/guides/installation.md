# Installation

{% hint style="info" %}
Please ensure you have done the [environment](environment-setup.md) setup before progressing.
{% endhint %}

The proof market tool chain is a collection of python scripts which allows users to interact with the proof market. These sets of scripts are required for all participants.

Clone the repository

```shell
git clone --recurse-submodules git@github.com:NilFoundation/proof-market-toolchain.git
```

Based on the user's role ,they need to install additional binaries/scripts to interact with the market.

## Circuit Developer

Please follow the installation guide for :

* [zkLLVM](https://docs.nil.foundation/zkllvm/guides/installation)&#x20;
  * &#x20;Used to compile your circuit and generate a binary to be hosted on proof market
* Set up [Authentication](../market/user-guides/sign-up.md) and follow the user guide for [circuit developers](../market/user-guides/).&#x20;

## Proof Requester

* [Lorem Ipsum CLI ](https://github.com/NilFoundation/lorem-ipsum-cli)
  * These could be smart contracts or zkLLVM for local validation.
* Setup [Authentication](../market/user-guides/sign-up.md) and follow the user guide for[ Proof Requesters ](../market/user-guides/)

## Proof Producer

As a proof producer, you need to compile the `proof-generator` binary. Currently, there are two variants, a single-threaded one (low performance) and a multi-threaded one.

### **Build**

```
cd proof-market-toolchain && mkdir build && cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang-12 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 ..
# Single-threaded version
cmake --build . -t proof-generator
# Multi-threaded version
cmake --build . -t proof-generator-mt
```

* Setup [Authentication](../market/user-guides/sign-up.md) and follow the user guide for [Proof Producers](../market/user-guides/)

