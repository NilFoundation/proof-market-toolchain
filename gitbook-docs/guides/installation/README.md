# Installation

{% hint style="info" %}
Please ensure you have done the [environment](../environment-setup.md) setup before progressing.
{% endhint %}

The proof market tool chain is a collection of python scripts which allows users to interact with the proof market. These set of scripts are required for all participants.

Clone the repository

```shell
git clone --recurse-submodules git@github.com:NilFoundation/proof-market-toolchain.git
```

Based on the users role ,they need to install additional binaries/scripts to interact with the market.

## Circuit Developer

Please follow the installation guide for :

* [zkLLVM](https://docs.nil.foundation/zkllvm/guides/installation)&#x20;
  * &#x20;Used to compile your circuit and generate a binary to be hosted on proof market
* Setup [Authentication](authentication.md) and follow the user guide for [circuit developers](../../market/user-role-guides/).&#x20;

## Proof Requester

* [Lorem Ipsum CLI ](https://github.com/NilFoundation/lorem-ipsum-cli)
  * These could be smart contracts or zkLLVM for local validation.
* Setup [Authentication](authentication.md) and follow the user guide for[ Proof Requesters ](../../market/user-role-guides/)

## Proof Producer

As a proof producer you need to compile the `proof-generator` binary. Currently there are two variants, a single threaded one (low performance) and a multi threaded.

### Dependencies

On \*nix systems, the following dependencies need to be installed

```
clang-12
clang++12
cmake >= 3.22.1
autoconf >= 2.71
automake >=  1.16.5
libc-ares-dev >= 1.18.1
libfmt-dev >= 8.1.1
liblz4-dev >= 1.9.3
gnutls-dev >= 7.81
libprotobuf-dev >= 3.12.4
libyaml-cpp-dev >= 0.2.2
libhwloc-dev >= 2.7.0
libsctp-dev >= 1.0.19
ragel >= 6.10
boost == 1.76
```

**We are aware of compilation issues with boost > 1.76 and clang > 12.0. Please use the versions recommended above**

### **Build**

```
cd proof-market-toolchain
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang-12 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 ..
# Single-threaded version
cmake --build . -t proof-generator
# Multi-threaded version
cmake --build . -t proof-generator-mt
```

* Setup [Authentication](authentication.md) and follow the user guide for [Proof Producers](../../market/user-role-guides/)

