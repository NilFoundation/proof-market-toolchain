# Requester/Producer Flow

In this guide, we will perform an end-to-end flow of interaction on the proof market between a proof requester and a proof producer for the `arithmetic` circuit. We will use the same user login but swap the _personas_ as we progress.

We do not cover the circuit/statement publishing flow.

## Pre-Requisites

Please ensure you have done the following steps:

* [Environment Setup](../guides/environment-setup.md): Install all dependencies.
* [Installation](../guides/installation.md):  Install and build `proof-generator-mt` binary.
* [Authentication](../market/user-guides/sign-up.md): Set up the `proof-market-toolchain` authentication.

We will execute all the commands from the home of the `proof-market-toolchain` repository.

## Get Statements

Let's start as a Proof Requester. A proof requester needs to put in a bid for a particular proof they are interested in. To do this, they first need to get hold of the key of the proof on the market.

We do this by getting all statements on the proof market.

```
python3 scripts/statement_tools.py get
```

{% hint style="info" %}
Please note all outputs are reduced for brevity, and only relevant attributes are shown here.
{% endhint %}

```
Statements:
 [
    {
        "_key": "32326",
        "_id": "statement/32326",
        "name": "arithmetic-example",
        "type": "placeholder-zkllvm",
        "description": "toy circuit example with basic arithmetic operations",
        "url": "https://github.com/NilFoundation/zkllvm",
        "input_description": "a, b from Pallas curve base field",
        "definition": {
            "proving_key": "; ModuleID = '...",
            "verification_key": "; ModuleID = '..."
        },
        ...
    }
```

The key of the arithmetic circuit is `32326`

{% hint style="info" %}
Please note the id might be different when you execute this flow, and you should substitute it as per your output.
{% endhint %}

## Send Bid

Proof Requester next sends a bid. Bid orders can optionally carry public inputs. In our case, we will send two numbers which we would like to be added on the Pallas curve. We will use a sample input stored in the `proof-market-toolchain` repository.

```
python3 scripts/bid_tools.py push --cost=2 --file=./example/input/arithmetic_example/input.json --key=32326
```

Where:

* _cost:_ Amount the requester is willing to pay in tokens/usd
* _file_: The path to the public input file.
* _key_: Identifier of the arithmetic circuit on the proof market.&#x20;

Output:

```
"Limit bid":{
   "_key":"16153352",
   "statement_key":"32326",
   "input":{
      "input":"1\n11"
   },
   "cost":2,
   "status":"created"
}
```

The important attributes in response are:

* _\_key_: Identifier of the order on the market. This identifier will be used to retrieve the order status and the proof once it is fulfilled.
* _status_: Status of the order. All orders start with the “_created_” state. It updates to the “_processing_” state when the order is matched & to “_completed_” when the proof is submitted. If the circuit/key combination already has the proof, the status will directly go to the _“completed”_ state.

The key of the bid order here is `16153352.`

This is now visible in the order book, and any Proof Producers can offer to pick this job.

### Check Bid Status

You can check your bid order status as follows. Please substitute the bid order key to reflect your order.

```
python3 scripts/bid_tools.py get --key=16153352
```

Output:

```
Bids:
 {
    "_key": "16153352",
    "cost": 2,
    "createdOn": 1675937905926,
    "eval_time": null,
    "input": {
        "input": "1\n11"
    },
    "statement_key": "32326",
    "status": "created",
}

```

## Send Ask

We will now send an Ask order as a proof producer for the same statement id.&#x20;

```
python3 scripts/ask_tools.py push --cost=2 --key=32326
```

Where&#x20;

* _cost_: Amount for which the proof producer can create a proof
* _key_: circuit/statement id (Arithmetic circuit id retrieved above)

Output:

```
"Limit ask":{
   "_key":"16153923",
   "statement_key":"32326",
   "cost":2,
   "status":"created"
}
```

The important attributes in response are:

* _\_key_: Identifier of the order on the market. This identifier will be used to retrieve the order status and the proof once it is fulfilled.
* _status_: Status of the order. All orders start with the “_created_” state. It updates to the “_processing_” state when the order is matched & to “_completed_” when the proof is submitted. If the circuit/key combination already has the proof, the status will directly go to the _“completed”_ state.

### Check Ask Status

```
python3 scripts/ask_tools.py get --key=16153923
```

Output:

```
Ask:
 {
    "_key": "16153923",
    "cost": 2,
    "eval_time": null,
    "statement_key": "32326",
    "status": "created",
}

```



## Wait for Matching

The matching engine of the proof market will try to match orders based on price (and generation time if provided).

Above we put the bid at 2 Tokens & ask at 2 Tokens. Hence these orders should be matched. We need to poll our orders to see if they have been matched. i.e. you need to retrieve the bid or ask orders and check the `status` field.&#x20;

{% hint style="info" %}
Your orders could get matched to different IDs and not the ones you posted, as it's decided by the matching engine on a number of parameters. For the time being, we assume the match was for the orders we posted.
{% endhint %}

### Check Bid Status

```
python3 scripts/bid_tools.py get --key=16153352
```

Output

```
 {
    "_key": "16153352",
    "ask_key": "16153923",
    "cost": 2,
    "statement_key": "32326",
    "status": "processing",
}

```

We see that `status` has changed to `processing` (the order has been matched). We also see `ask_key` populated to the matched ask order.

### Check Ask Status

```
python3 scripts/ask_tools.py get --key=16153923
```

Output:

```
Ask:
 {
    "_key": "16153923",
    "bid_key": "16153352",
    "cost": 2,
    "matched_time": 1675938454814,
    "statement_key": "32326",
    "status": "processing",
}

```

We see that `status` has  changed to `processing` (the order has been matched). We also see `bid_key` populated to the matched bid order.

## Generate Proof

Once the orders have matched, as a proof producer, you should now begin the process of creating the proof for the circuit.

```
./build/bin/proof-generator-mt/proof-generator-mt --proof_out=arithmetic_proof.bin --circuit_input=example/statements/arithmetic_example_statement.json --public_input=example/input/arithmetic_example/input.json --smp=2
```

* _proof\_out_: Location to store output proof file
* _circuit\_input:_ Circuit statement from proof market
* _public\_input:_ Public inputs of the MINA state used in the bid.
* _smp : (Multi-threaded only) Number of threads to spawn_

Ideally, you should retrieve the `circuit_input` and `public_input` from the proof market. See [here](../market/user-guides/proof-producer.md#order-status-fetch-inputs).

Output:

```
Inner verification passed
0
```

You should now have a `arithmetic_proof.bin` file in your directory, assuming no errors were encountered.&#x20;

## Publish Proof

```
python3 scripts/proof_tools.py  push -a 16153923 -f arithmetic_proof.bin
```

Output:

```
Proof is pushed
```

### Check Bid Status

```
python3 scripts/bid_tools.py get --key=16153352
```

Output:

```
Bids:
 {
    "_key": "16153352",
    "ask_key": "16153923",
    "cost": 2,
    "proof_key": "16158141",
    "statement_key": "32326",
    "status": "completed",
    "updatedOn": 1675942301032,
 }
```

### Check Ask Status

```
python3 scripts/ask_tools.py get --key=16153923
```

Output

```
Ask:
 {
    "_key": "16153923",
    "bid_key": "16153352",
    "cost": 2,
    "proof_key": "16158141",
    "statement_key": "32326",
    "status": "completed",
}
```

In both bid/ask orders, we can now see the following.

* `status` updated from `processing` to `completed` : This implies the proof is submitted
* `proof_key` : This attribute has the `key` of the proof that the proof requester can use to retrieve the submitted proof by the proof producer.

Congratulations! If you have made it this far, you have managed a full interaction between the proof producer and a proof requester.
