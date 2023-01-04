---
description: Command line guide to proof market
---

# End to End flow





In this guide , we walk through all the steps to interact with the proof market. We will cover everything from compiling a circuit , placing/matching order , generating/verifying proof.

We will use two [personas](../overview.md#entities) defined earlier&#x20;

* Proof Generator (PG)&#x20;
* Proof Requester (PR)

We require to setup two projects :&#x20;

* Proof Market tool-chain  (`toolchain-env`) : Please follow [environment setup](../../guides/environment-setup.md) & [installation](../../guides/installation.md) guide.
* zkLLVM (`zkllvm-env`): Please follow [environment setup](https://nil-foundation.gitbook.io/zkllvm/guides/environment-setup) & [installation guide](https://nil-foundation.gitbook.io/zkllvm/guides/installation).

We will perform the following steps with the personas above  and the environment in which the commands carried out in `toolchain-env` or `zkllvm-env` environment

1. [Proof Requester - Compile Circuit](./#1.-pr-compile-circuit) : Compile circuit to generate byte-code. _(executed by PR)_
2. [Proof Requester - Push order to Proof Market](./#2.-pr-push-order-to-proof-market): Push order to proof market with bytecode & public inputs (if any) _(executed by PR)_
3. [Proof Generator -  Match Order & Generate Proof](./#3.-pg-match-order-and-generate-proof): Accept order from market and generate proof _(executed by PG)_
4. [Proof Generator - Push Proof to the Proof Market](./#4.-pg-push-proof-to-proof-market): Build zkLLVM & compile circuit to generate byte-code _(executed by PG)_
5. [Proof Requester - Verify Proof](./#5.-pr-verify-proof): Build zkLLVM & compile circuit to generate byte-code _(executed by PR)_



### 1. Proof Requester -  Compile Circuit

We will first compile the circuit from one of the examples.

{% hint style="success" %}
Please change your active environment to `zkllvm-env`
{% endhint %}

* cmake configure&#x20;

```shell
cmake -GNinja -B ${CIRCUIT_BUILD:-circuit_build} -DZKLLVM_BUILD=True -DCC=”${ZKLLVM_BUILD:-build}/libs/circifier/llvm/bin/clang”  -DCMAKE_BUILD_TYPE=Release .
```

* Generate the byte-code from the circuit

```shell
ninja -C ${CIRCUIT_BUILD:-circuit_build} zkllvm_examples_posseidon -j$(nproc)
```

This will output the circuit byte-code `circuit.bc`



### **2. Proof Requester -  Push order to Proof Market**

We will take the byte-code from the previous step and create an order and push it to the proof market.

{% hint style="success" %}
Please change your active environment to `toolchain-env`
{% endhint %}

* Push order to proof market

```shell
python ./scripts/order_push.py -bc ${ZKLLVM_DIR:-../zkllvm}/${CIRCUIT_BUILD:-build}/circuit.bc -pi public_input.inp …(some other params) 
```

The response to this step will be the order details

TODO - ADD details

### **3. Proof Generator - Match Order & Generate Proof**

{% hint style="info" %}
In the current version order matching is done via a daemon and no user interaction is required.  (TODO - CHECK)
{% endhint %}

{% hint style="success" %}
Please change your active environment to `toolchain-env`
{% endhint %}

* The proof generator needs retrieve the order matched by executing the following

```shell
python ./scripts/order_get.py …()
```

Once the order is retrieved , the proof generator needs to run `assigner` which is part of the zkLLVM project.

{% hint style="success" %}
Please change your active environment to `zkllvm-env`
{% endhint %}

* Run assigner

```shell
${ASSIGNER_BUILD:-build}/bin/assigner/assigner circuit.bc -i public_input.bc

```

This produces two outputs of `execution_trace.bin` and `constraints.bin`

The proof can be produced using the above two files by executing the following from the tool chain environment (TODO improve doc)

{% hint style="success" %}
Please change your active environment to `toolchain-env`
{% endhint %}

* Generate proof&#x20;

```
 bin/state-proof-gen -cs constraint_system.bin -ex execution_trace.bin -o proof.bin
```

This creates the proof `proof.bin`

### **4.  Proof Generator -  Push Proof to Proof Market**

{% hint style="success" %}
Please change your active environment to `toolchain-env`
{% endhint %}

* Push the proof to the proof market

```
python proof_push.py -o proof.bin
```

### **5.  Proof Requester-  Verify Proof**

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

