# =nil; Proof Market Proof Generator

# Introduction

This repository serves as a PoC of how a =nil; Foundation supported proof market would operate.

The marketplace consists of the following entities.
- Proof Requester : This can be an application like a bridge requesting 
balance or a user interested in cross cluster operation and/or trust-less data access.
- Proof Generator : This is an entity who will generate the proofs for the requests/orders
made by the Proof Requester.
- Circuit Developer : This is an entity who prepare circuits for proof market

Below we will list a set of operations a user can follow along which demonstrates the market 
operation interaction between the above two entities.

# Proof Market Interaction

## 0. Prepare zkLLVM (for circuit developers)
The most productive way to create a circuit definition for Proof Market is [zkLLVM](https://github.com/NilFoundation/zkllvm).
All dependencies and build instructions are inside the [zkLLVM's repository](https://github.com/NilFoundation/zkllvm). 

## 1. Circuit Generation/Publishing (for circuit developers)

Circuits can be generated by any one. They are serialised &  published on the proof market. 
This allows for reuse of the circuits by all other proof requesters. Circuits need a set 
of public inputs. Below we create a new circuit and public params for it.

1.1  Create a new сircuit (using [zkLLVM](https://github.com/NilFoundation/zkllvm))
```
make -C ${ZKLLVM_BUILD:-build} <circuit target name> -j$(nproc)
```

1.2 Prepare circuit description for Proof Market

Circuits are stored as statement structure on Proof Market.
Statement description example can be found in /example directory

```
python3 scripts/prepare_circuit.py -c <zkllvm output> -o <statement description file>
```

1.2 Publish it to Proof Market

This circuit can now be pushed to the Proof market via python script

```
python3 scripts/statement_push.py --file <json file with statement description> --auth <json authorization file>
```

Authorization file examples can be found in /example directory. 

## 2. Proof Market Bid Creation

The proof requester can create an order. Order
has additional details such as who is requesting the proof , what are they willing to pay for it and

```
python3 scripts/bid_push.py --cost <cost of the bid> --file <public_input_file> --key <key of the statement> --auth <json authorization file>
```

The proof requester can check their bid with
```
python3 scripts/bid_get.py -k <key of the bid> --auth <json authorization file>
```


## 3. Wait for asks
Here the proof requester waits for proposals to be submitted against this order.


## 4. Submit ask
While the proposal is up, we now view at the marketplace from the perspective of a 
proof producer. In steps 1-3 , the requester put out a bid. Now the producer can
observe them in the marketplace and start replying with an ask.

```
python3 scripts/ask_push.py --cost <cost of the ask> --key <key of the statement> --auth <json authorization file>
```

The proof producer can check their ask with:
```
python3 scripts/ask_get.py -k <key of the ask> --auth <json authorization file>
```


## 5. Order Matching

Proof Market runs mathcing algorithm between bids and asks for each new bid/ask. 
It chooses the cheapest ask that fits the requirenments from the proof requester. 


## 6. Proof Generation
Ask's status 'processing' means that the ask was mathed with a bid.
Now it is time to generate a proof for proof producer. 

First of all, the proof producer needs circuit definition:
```
python3 scripts/circuit_get.py --key <statement key for the bid> -o <output file> --auth <json authorization file>
```

Next, public input of the bid:
```
python3 scripts/public_input_get.py --key <bid key> -o <output file path> --auth <json authorization file>
```

Execute the below to generate a proof (using [zkLLVM](https://github.com/NilFoundation/zkllvm)):
```
${ZKLLVM_BUILD:-build}/bin/assigner/assigner -b <circuit definition file> -i <input file> -t <assignments table file> -c <proof file>
```


## 7. Proof Submission
The proof generator can now submit the proof to the marketplace, where if verified, they will
get the reward.

```
python3 scripts/proof_push.py --bid_key <key of the bid> --ask_key <key of the ask> --file <file with the proof> --auth <json authorization file>
```

## 8. Get Proof

Now the proof requester is able to get their proof either by bid key or proof key.

```
python3 scripts/proof_get.py --bid_key <key of the bid> --auth <json authorization file>
```

```
python3 scripts/proof_get.py --key <key of the proof> --auth <json authorization file>
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
