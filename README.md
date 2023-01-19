# =nil; Proof Market Toolchain

[![Discord](https://img.shields.io/discord/969303013749579846.svg?logo=discord&style=flat-square)](https://discord.gg/KmTAEjbmM3)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=flat-square&logo=telegram&logoColor=dark)](https://t.me/nilfoundation)
[![Twitter](https://img.shields.io/twitter/follow/nil_foundation)](https://twitter.com/nil_foundation)


# Introduction

This repository serves as a PoC of how a =nil; Foundation built [proof market](https://proof.market/) would operate.
This repository also provides a set of scripts/tools required to participate in the proof market.

The marketplace consists of the following entities.
- Proof Requester : This can be an application like a bridge requesting 
balance or a user interested in cross cluster operation and/or trust-less data access.
- Proof Producer : This is an entity who will generate the proofs for the requests/orders
made by the Proof Requester.
- Circuit Developer : This is an entity who prepare circuits for proof market

# Dependencies

On *nix systems, the following dependencies need to be installed

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
   

# Installation

- Clone the repo
 ```
git clone git clone --recurse-submodules git@github.com:NilFoundation/proof-market-toolchain.git
cd proof-market-toolchain
```
You need to build the proof generator only if you wish to generate proofs.

#### Building 
```
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang-12 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-12 ..
# Single-threaded version
cmake --build . -t proof-generator
# Multi-threaded version
cmake --build . -t proof-generator-mt
```

# Proof Market Beta Access

Please navigate [here](https://docs.nil.foundation/proof-market/market/front-end#new-user-signup) to see a guide on how to get access for proof market.

Once you have the credentials, user needs to create two files in the `scripts/` directory  with the following information. 
-  .user
 ```
username
```

- .secret
```
  password
  ```
# Proof Market Interaction

Below we will list a set of operations a user can follow along which demonstrates the market
operation interaction between the above entities.

## 1. Prepare zkLLVM (for circuit developers)
The easiest way to create a circuit definition for Proof Market is [zkLLVM](https://github.com/NilFoundation/zkllvm).
All dependencies and build instructions are inside the [zkLLVM's repository](https://github.com/NilFoundation/zkllvm).
If you wish to interact with existing circuits/statements, please go to step 3. 

## 2. Circuit Generation/Publishing (for circuit developers)

Circuits can be generated by any one. They are serialised &  published on the proof market. 
This allows for reuse of the circuits by all other proof requesters. Circuits need a set 
of public inputs. 

2.1  Create a new circuit (using [zkLLVM](https://github.com/NilFoundation/zkllvm))
```
make -C ${ZKLLVM_BUILD:-build} <circuit target name> -j$(nproc)
```


2.2 Prepare a statement with circuit description for Proof Market (using this repository)

Circuits are stored as a *statement* structure on Proof Market.
Statement description example can be found in `example/statements/` directory

```
python3 scripts/prepare_statement.py -c <zkllvm output> -o <statement description file>
```

Provide the necessary information listed in the output statement file

2.2 Publish it to Proof Market

This statement can now be pushed to the Proof market via python script

```
python3 scripts/statement_tools.py push --file <json file with statement description> --auth <json authorization file>
```

You will be returned an object containing a *_key* filed -- unique descriptor of the statement

2.3 Get all statements

A list of all available statements can be obtained by

```
python3 scripts/statement_tools.py get
```

## 3. Proof Market Bid Creation

The proof requester can create a bid order. It
has additional details such as what are they willing to pay for it and public inputs.


```
python3 scripts/bid_tools.py push --cost <cost of the bid> --file <json file with public_input> --key <key of the statement> 
```

The proof requester can check their bid with
```
python3 scripts/bid_tools.py get --key <key of the bid> 
```


## 4. Wait for asks
Here the proof requester waits for matching engine to either match an existing order or wait for proposals to be submitted against this circuit/statement. 


## 5. Submit ask
While the proposal is up, we now view at the marketplace from the perspective of a 
proof producer. In steps 2-4 , the requester put out a bid. Now the producer can
observe them in the marketplace and start replying with an ask.

```
python3 scripts/ask_tools.py push --cost <cost of the ask> --key <key of the statement> 
```

The proof producer can check their ask with

```
python3 scripts/ask_tools.py get --key <key of the ask> 
```


## 6. Order Matching

Proof Market runs a matching algorithm between bids and asks for each new bid/ask. It chooses the cheapest ask that fits the requirements of the proof requester. 

## 7. Proof Generation
Ask's status 'processing' means that the ask was matched with a bid.
Now it is time to generate a proof for the proof producer. 

First of all, the proof producer needs circuit definition:

```
python3 scripts/statement_tools.py get --key <key of the statement> -o <output file> 
```

Next, public input of the bid:

```
python3 scripts/public_input_get.py --key <bid key> -o <output file path> 
```

Execute the below to generate a proof:
```
cd build
./bin/proof-generator/proof-generator --proof_out=<output file> --circuit_input=<statement from Proof Market> --public_input=<public input from Proof Market>
```
or
```

cd build
./bin/proof-generator/proof-generator-mt --proof_out=<output file> --circuit_input=<statement from Proof Market> --public_input=<public input from Proof Market> --smp=<number of threads>

```

Readme for Proof Producer daemon in located [here](./proof_producer/README.md).

## 8. Proof Submission
The proof generator can now submit the proof to the marketplace, where if verified, they will
get the reward.

```
python3 scripts/proof_tools.py push --bid_key <key of the bid> --ask_key <key of the ask> --file <file with the proof> 
```

You can provide only one of two possible keys

## 9. Get Proof

Now the proof requester is able to get their proof either by bid key or proof key.

```
python3 scripts/proof_tools.py get --bid_key <key of the bid> 
```

Validation of the proof is not part of the tool chain. Validation flow is implemented in the 
lorem ipsum cli repository [here](https://github.com/NilFoundation/lorem-ipsum-cli).

# Common issues
## Compilation Errors
If you have more than one compiler installed i.e g++ & clang++. The make system might pick up the former. You can explicitly force usage of
clang++ by finding the path and passing it in the variable below.

```
`which clang++`  
cmake .. -DCMAKE_CXX_COMPILER=<path to clang++ from above>
```

## Submodule management
Git maintains a few places where submodule details are cached. Sometimes updates do not come through. ex: Deletion , updating
a url of a previously checked out submodule.It is advisable to check these locations for remains or try a new checkout.
- .gitmodules
- .git/config
- .git/modules/*

## Compilation errors for Proof generator
Please ensure you are using 1.76 version of boost as the higher versions have an incompatible API which will
be updated in due course.

## macOS
On macOS, these dependencies are required for compilation
```
fmt gnutls protobuf yaml-cpp ragel hwloc
```


# Documentation

Documentation portal for proof market is located [here](https://docs.nil.foundation/proof-market).
Users are encouraged to check [zkLLVM](https://github.com/NilFoundation/zkllvm) project which is tightly coupled to the proof market.

## Support

Additional support can be obtained by contact the team at [Telegram](https://t.me/nilfoundation) and [Discord](https://discord.gg/KmTAEjbmM3).

