# =nil; Proof Market (PoC)

# Introduction

This repository serves as a PoC of how a nil foundation supported proof market would operate.

The marketplace consists of the following entities.
- Proof Requester : This can be an application like a bridge requesting 
balance or a user interested in cross cluster operation and/or trust-less data access.
- Proof Generator : This is an entity who will generate the proofs for the requests/orders
made by the Proof Requester.

Below we will list a set of operations a user can follow along which demonstrates the market 
operation interaction between the above two entities.


# Dependencies

- [Boost](https://www.boost.org/) >= 1.74.0
- [cmake](https://cmake.org/) >= 3.5
- [clang](https://clang.llvm.org/) >= 14.0.6

On *nix systems, the following dependencies need to be present & can be installed using the following command

```
 sudo apt install build-essential libssl-dev libboost-all-dev cmake clang git
```


## Installation

Clone the repository
```
git clone git@github.com:NilFoundation/placeholder-proof-gen.git
cd placeholder-proof-gen.
```

- Clone all submodules recursively
```
git submodule update --init --recursive
```








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