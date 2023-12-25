# CLI repository for =nil; Proof Market

[![Discord](https://img.shields.io/discord/969303013749579846.svg?logo=discord&style=flat-square)](https://discord.gg/KmTAEjbmM3)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=flat-square&logo=telegram&logoColor=dark)](https://t.me/nilfoundation)
[![Twitter](https://img.shields.io/twitter/follow/nil_foundation)](https://twitter.com/nil_foundation)

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Signing up on the Proof Market](#signing-up-on-the-proof-market)
- [Proof Market Interaction](#proof-market-interaction)
  - [1. Prepare zkLLVM (for circuit developers)](#1-prepare-zkllvm-for-circuit-developers)
  - [2. Circuit Generation/Publishing (for circuit developers)](#2-circuit-generationpublishing-for-circuit-developers)
  - [3. Requesting the proof](#3-requesting-the-proof)
  - [4. Awaiting for Proposals](#4-awaiting-for-proposals)
  - [5. Submit proposal](#5-submit-proposal)
  - [6. Order Matching](#6-order-matching)
  - [7. Proof Generation](#7-proof-generation)
  - [8. Proof Submission](#8-proof-submission)
  - [9. Obtaining the Proof](#9-obtaining-the-proof)
  - [10. Verifying the Proof](#10-verifying-the-proof)
- [Common issues](#common-issues)
  - [macOS](#macos)
- [Community](#community)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Introduction

This repository provides a set of scripts and tools required to participate in the
`=nil;` Foundation's [Proof Market](https://proof.market/). This is a new version of the proof market. 
It is backward compatible with the old version, but the API is slightly different.
If you are looking for the old version, please go to the [vanilla branch](https://github.com/NilFoundation/proof-market-toolchain/tree/vanilla).



There are three primary roles (parties) in the Proof Market:

- **Proof requesters** are applications that require zero-knowledge proofs,
  and make requests for them on the Proof Market.
- **Proof producers** are owners of computational infrastructure, who 
  generate proofs for the requests made by proof requesters.
- **Circuit developers** make zero-knowledge circuits, that are used to generate
  requests and subsequent proofs.

The Proof Market toolchain has tools for proof requesters and producers.
To learn more about the Proof Market and these roles, read the 
[Proof Market documentation](https://docs.nil.foundation/proof-market).

If you're interested in circuit development, check out the
[zkLLVM template project](https://github.com/NilFoundation/zkllvm-template) and
[zkLLVM toolchain docs and tutorials](https://docs.nil.foundation/zkllvm).

# Prerequisites

The CLI is written in `Python 3.8`. Depending on the role in which you want to participate, 
you will also need to have the toolchain binaries installed
on your system.

## Environment setup for proof requesters

As a proof requester, you will order proofs of the existing circuits from the Proof Market. 
The proofs are being verified on the Proof Market as part of the protocol, but you can also
verify them locally or on EVM.

To verify proofs locally, you will need to install the following dependencies:
* proof-verifier

To verify proofs on EVM, you can use our [EVM verifier](https://github.com/NilFoundation/evm-placeholder-verification/).

## Environment setup for proof producers

As a proof producer, you will generate proofs for the requests made by proof requesters. 
You can use the default proof producer service, or implement your own.

To use the default proof generator, you will need to install it from the deb package:

```bash
echo 'deb [trusted=yes]  http://deb.nil.foundation/ubuntu/ all main' >>/etc/apt/sources.list
apt update
apt install -y proof-generator zkllvm
```

This will install the proof generator service with dependencies and allow you to participate in the proof 
generation process.

## Environment setup for circuit developers

Circuit developers play a crucial role in the Proof Market. They create zero-knowledge circuits, 
that can be used privately or publicly by zk-enabled applications.

If you're interested in circuit development, check out the
[zkLLVM template project](https://github.com/NilFoundation/zkllvm-template) and
[zkLLVM toolchain docs and tutorials](https://docs.nil.foundation/zkllvm).

Environment setup for circuit developers is described in the [zkLLVM repository](https://github.com/NilFoundation/zkLLVM). 
In the case of binary distribution, you can install everything from the deb package:

```bash
echo 'deb [trusted=yes]  http://deb.nil.foundation/ubuntu/ all main' >>/etc/apt/sources.list
apt update
apt install -y zkllvm
```

If you want to use our [template repository](https://github.com/NilFoundation/zkllvm-template) to start
your circuit development, you will also need to install the following dependencies:

```bash
apt install -y zkllvm cmake libboost-all-dev
```


# Signing up on the Proof Market

Before proceeding with the Proof Market interaction, make sure you have installed the dependencies 
required for the role you want to participate in.

We recommend using virtualenv when using the scripts:

```bash 
pip3 install --user virtualenv
python3 -m virtualenv venv
source venv/bin/activate
pip3 install -r requirements.txt
```

Please [see the documentation](https://docs.nil.foundation/proof-market/market/front-end#new-user-signup)
on how to get access for proof market.
Or navigate to `scripts/` directory and run the following commands.

First, create a username and password.
Store them in files `.user` and `.secret`:

```console
$ cd scripts
$ echo '<username>' > .user
$ echo '<password>' > .secret
```

Signup to the proof market with the following command.
You'll need to repeat your username and password here.
```
$ python3 signup.py user -u <username> -p <password> -e <e-mail>
```

This command will return user metadata in JSON format:

```json
{
  "user": "username",
  "active": true,
  "extra": {},
  "error": false,
  "code": 201
}
```

If you wish to submit generated proofs to the Proof Market,
you need to also register as a proof producer:

```console
$ python3 signup.py producer --url https://example.com
```

It will return producer metadata in JSON format:

```json
{
  "_key": "...",
  "_id": "producer/...",
  "_rev": "_fneT7J2---",
  "description": "Generic Producer",
  "url": "https://example.com",
  "name": "username",
  "createdOn": 1677588821180,
  "updatedOn": 1677588821180
}
```

# Proof Market Interaction

Below we will list a set of operations a user can follow along which demonstrates the market
operation interaction between the above entities.

## 1. Prepare zkLLVM (for circuit developers)

The easiest way to create a circuit definition for Proof Market is [zkLLVM](https://github.com/NilFoundation/zkllvm).
All dependencies and build instructions are inside the [zkLLVM's repository](https://github.com/NilFoundation/zkllvm).
If you wish to interact with existing circuits/statements, please go to step 3.

## 2. Circuit Generation/Publishing (for circuit developers)

Circuits can be generated by any one. They are serialised & published on the proof market.
This allows for reuse of the circuits by all other proof requesters. Circuits need a set
of public inputs.

2.1 Create a new circuit (using [zkLLVM](https://github.com/NilFoundation/zkllvm))

```
make -C ${ZKLLVM_BUILD:-build} <circuit target name> -j$(nproc)
```

2.2 Prepare a statement with circuit description for Proof Market (using this repository)

Circuits are stored as a *statement* structure on Proof Market.
Statement description example can be found in `example/statements/` directory

Please ensure you have selected the option to create *ll IR files in the [zkLLVM setup](https://github.com/NilFoundation/zkllvm#2-configure-cmake).

```bash
python3 scripts/prepare_statement.py \
    -c <zkllvm output> \
    -o <statement description file> \
    -n <statement name> \
    -t <statement type> \
    --private | --public
```
The `--private` or `--public` parameters are mutually exclusive:

* With `--private`, the statement will be accessible only by its ID.
  For experiments and development purposes, make your statements private.
* With `--public`, the statement will be openly listed on the Proof Market.

Provide the necessary information listed in the output statement file

2.2 Publish it to Proof Market

This statement can now be pushed to the Proof market via python script

```
python3 scripts/statement_tools.py push --file <json file with statement description>
```

You will be returned an object containing a *_key* filed -- unique descriptor of the statement

2.3 Get all statements

A list of all available statements can be obtained by

```
python3 scripts/statement_tools.py get
```

## 3. Requesting the proof

The proof requester can create a request order. It
has additional details such as what are they willing to pay for it and public inputs.

```
python3 scripts/request_tools.py push --cost <cost of the request> --input <json file with public_input> --statement-key <key of the statement> 
```

The proof requester can check their request with

```
python3 scripts/request_tools.py get --request-key <key of the request> 
```

### Requesting a proof in an aggregated manner

For some circuits it makes sense to request proofs in an aggregated manner. 
More on this can be found in the [zkLLVM hints on circuits writing](https://docs.nil.foundation/zkllvm).

To request a proof in an aggregated manner, you need to use slightly different command:

```bash
python3 aggregated_request.py --cost <cost of the request> --input <json file with public_input> --statement-key <key of the statement> --aggregation-ratio 4
```

## 4. Awaiting for Proposals

Here the proof requester waits for matching engine to either match an existing order or wait for proposals to be submitted against this circuit/statement.

## 5. Submit proposal

While the proposal is up, we now view at the marketplace from the perspective of a
proof producer. In steps 2-4 , the requester put out a request. Now the producer can
observe them in the marketplace and start replying with an proposal.

```
python3 scripts/proposal_tools.py push --cost <cost of the proposal> --key <key of the statement> 
```

The proof producer can check their proposal with

```
python3 scripts/proposal_tools.py get --key <key of the proposal> 
```

## 6. Order Matching

Proof Market runs a matching algorithm between requests and proposals for each new request/proposal. It chooses the cheapest proposal that fits the requirements of the proof requester.

## 7. Proof Generation

Proposal's status 'processing' means that the proposal was matched with a request.
Now it is time to generate a proof for the proof producer.

First of all, the proof producer needs to obtain assigned requests:

```bash
python3 proof_producer.py prepare --db-path ./db
```

After that, execute the below to generate a proof:

```bash
python3 proof_producer.py start --proof-generator /path/to/proof-generator --assigner /path/to/assigner --db-path ./db --log-level info
```

If you have the directory with the proof generator in your PATH, you can use:
```bash
python3 proof_producer.py start --proof-generator proof-generator --assigner assigner --db-path ./db --log-level info
```

Readme for Proof Producer daemon in located [here](./proof_producer/README.md).

## 8. Proof Submission

The proof generator can now submit the proof to the marketplace, where if verified, they will
get the reward.

```
python3 scripts/proof_tools.py push --request_key <key of the request> --proposal_key <key of the proposal> --file <file with the proof> 
```

You can provide only one of two possible keys

## 9. Obtaining the Proof

Now the proof requester is able to get their proof either by request key or proof key.

```
python3 scripts/proof_tools.py get --request_key <key of the request> 
```

Validation of the proof is not part of the tool chain. Validation flow is implemented in the
lorem ipsum cli repository [here](https://github.com/NilFoundation/lorem-ipsum-cli).

## 10. Verifying the Proof

The proof requester can verify the proof locally with the following command:

```bash
/path/to/proof-verifier --proof <proof file> --statement <statement file> --public-input <public input file>
```

You can also verify the proof on EVM with our [EVM verifier](https://github.com/NilFoundation/evm-placeholder-verification/) by following the instructions in the repository.

# Common issues

## macOS

On macOS, we currently have only the experimental support for both proof generator and zkllvm. If you want to interact 
with the Proof Market on macOS in any role apart from proof requester, you will need to build the required tools from source.

For zkllvm you can follow the [zkllvm repository](https://github.com/NilFoundation/zkLLVM#building) instructions.

For proof generator you can follow the instructions in the [proof generator repository](https://github.com/NilFoundation/proof-producer/) to build it from source.

# Community

Join our community on [Telegram](https://t.me/nilfoundation) and [Discord](https://discord.gg/KmTAEjbmM3).

