---
description: Command line guide to proof market
---

# Command Line

In this guide , we walk through all the steps to interact with the proof market. We will cover everything from compiling a circuit , placing/matching order , generating/verifying proof.

We will use the [personas](overview.md#entities) defined earlier as Proof Generator (PG) and Proof Requester (PR).

1. `[PR]` [Compile Circuit ](command-line.md#compile-circuit): Build zkLLVM & compile circuit to generate bytecode
2. `[PR]`[Push order to Proof Market](command-line.md#push-order-to-proof-market): Push order to proof market with bytecode & public inputs (if any)
3. `[PG]` [Match Order & Generate Proof](command-line.md#match-order-and-generate-proof): Accept order from&#x20;
4. `[PG`] [Push Proof to the Proof Market](command-line.md#push-proof-to-proof-market): Build zkLLVM & compile circuit to generate bytecode
5. `[PR]`[Verify Proof](command-line.md#push-proof-to-proof-market-1): Build zkLLVM & compile circuit to generate bytecode



### 1. \[PR] Compile Circuit

(TODO - check if its easier to copy steps instead of linking)

In order to compile a circuit, we will first need to build & install the [zkLLM](https://nil-foundation.gitbook.io/zkllvm/) compiler.&#x20;

Please follow the following steps:

1. [Install Dependencies required by zkLLVM](https://nil-foundation.gitbook.io/zkllvm/guides/environment-setup)
2. [Build modified zkLLVM clang and assigner.](https://nil-foundation.gitbook.io/zkllvm/guides/installation)

Once the binaries are ready. We will next compile an example in the zkLLVM repository.

* cmake configure the environment.

```shell
cmake -GNinja -B ${CIRCUIT_BUILD:-circuit_build} -DZKLLVM_BUILD=True -DCC=”${ZKLLVM_BUILD:-build}/libs/circifier/llvm/bin/clang”  -DCMAKE_BUILD_TYPE=Release .
```

* Generate the bytecode from the circuit

```shell
ninja -C ${CIRCUIT_BUILD:-circuit_build} zkllvm_examples_posseidon -j$(nproc)
```



### **2.\[PR] Push order to Proof Market**

To push this circuit to the proof market , we need scripts which are in the tool-chain. Please follow the steps:

1. [Install dependencies required by Proof Market toolchain](../guides/environment-setup.md)
2. TODO  - Check if any build step?

```shell
python ./scripts/order_push.py -bc ${ZKLLVM_DIR:-../zkllvm}/${CIRCUIT_BUILD:-build}/circuit.bc -pi public_input.inp …(some other params) 
```

The response to this step will be the order details

TODO - ADD details

### **3. \[PG] Match Order & Generate Proof**

{% hint style="info" %}
In the current version order matching is done via a daemon and no user interaction is required.  (TODO - CHECK)
{% endhint %}

The proof generator needs retrieve the order matched by executing the following

```shell
python ./scripts/order_get.py …()
```

Once the order is retrieved , the proof generator needs to run `assigner` which is part of the zkLLVM project.

TODO - Check&#x20;

```shell
${ASSIGNER_BUILD:-build}/bin/assigner/assigner circuit.bc -i public_input.bc

```

This produces two outputs of an `execution_trace.bin` and `constraints.bin`

The proof can be produced using the above two files by executing the following from the tool chain environment (TODO improve doc)

```
 bin/state-proof-gen -cs constraint_system.bin -ex execution_trace.bin -o proof.bin
```

This creates the proof `proof.bin`

### **4. \[PG] Push Proof to Proof Market**

The proof can be pushed to the proof market with the following command. (TODO improve doc)&#x20;

```
python proof_push.py -o proof.bin
```

### **5. \[PR] Verify Proof**

The proof can now be verified by the Proof Requester&#x20;

TODO - How does the proof requester retrieve the proof

The following script prepares the proof to be verified on chain

```
python ./scripts/verifier_input_get.py -assigner_path “./assigner” -order order.bin -proof proof.bin …() 
```

TODO add EVM steps

****

****

****

****

****

