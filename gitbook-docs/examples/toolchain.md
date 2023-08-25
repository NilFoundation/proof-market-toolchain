# Interacting with Proof Market toolchain

In this guide, we will perform an end-to-end interaction on Proof Market
between a proof requester and a proof producer for the `arithmetic-example` circuit.
We will use the same user but swap roles as we progress.

We won't cover circuit/statement publishing here, you can learn about it
in the [CLI section](../toolchain/cli-reference/statement.md#building-statements-from-circuits).

## Prerequisites

Prerequisites for this guide are
[Proof Market toolchain installation](../toolchain/installation.md)
and [authentication on Proof Market](../toolchain/sign-up.md).

We will execute all the commands from the `proof-market-toolchain` repository's home.

## Get all statements

Let's start as a proof requester who needs to put in a request for a particular proof
they are interested in.
First, they need that proof's id (key) from the market.

We'll get all statements on Proof Market and find the required one.

{% hint style="info" %}
Note that all outputs are reduced for brevity, and only relevant attributes are shown here.
{% endhint %}

```console
$ python3 scripts/statement_tools.py get

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

The id/key of the arithmetic circuit's statement is `32326`.
Note that the id might be different when you run this flow.
In that case, you should substitute it as per your output.

## Send a request

The statement id is retrieved, now the proof requester can send a request.
Request orders can optionally carry public inputs.
For our example, we'll send two numbers we want to be added on the Pallas curve.
We will use a sample input from the `proof-market-toolchain` repository:

```bash
$ python3 scripts/request_tools.py push \
    --cost 2 \
    --file ./example/input/arithmetic_example/input.json \
    --key 32326

"Limit request":{
   "_key":"16153352",
   "statement_key":"32326",
   "input":{
      "input":"1\n11"
   },
   "cost":2,
   "status":"created"
}
```

Attributes of the request are:
   * `key` — the key (id) of the statement for which this order is placed;
   * `cost` (in USD/tokens) — the amount that the proof requester is willing to pay for the proof;
   * `file` (file path) — the path to the public input file.

The attributes we're interested in this response are:
   * `_key` — the id of the placed order, we will need it to check the order's status
     and get the proof once it's ready;
   * `status` — the order's status.
     All orders start in the `created` state, which updates to the `processing` state
     when the order is matched to a proof producer.
     Once the proof is submitted to the market, order's state changes to `completed`.
     If proof for this combination of statement and public input already exists on the market,
     the order's status will directly go to the `completed` state.

The key of the request order here is `16153352`.
This order is now visible in the order book, and any proof producer can offer
to pick up this job.

### Check request status

You can check your request status as follows.
Remember to substitute the order key to reflect your order.

```console
$ python3 scripts/request_tools.py get --key 16153352

Requests:
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

## Send a proposal

We will now change the role and send a proposal order in response to the request
as a proof producer.

```console
$ python3 scripts/proposal_tools.py push \
    --cost 2 \
    --key 32326

"Limit proposal":{
   "_key":"16153923",
   "statement_key":"32326",
   "cost":2,
   "status":"created"
}
```

Attributes of the proposal are:
   * `key` — the key (id) of the statement for which this order is placed;
   * `cost` (in USD/tokens) — the amount that the proof producer is willing to create a proof for.

The attributes of the response are the same as for [request orders](#send-a-request).

### Check proposal status

You can check your proposal status the same way as with request orders,
only with the `proposal_tools.py` script:

```console
$ python3 scripts/proposal_tools.py get --key 16153923

Proposal:
 {
    "_key": "16153923",
    "cost": 2,
    "eval_time": null,
    "statement_key": "32326",
    "status": "created",
}
```

## Wait for matching

The matching engine of Proof Market will try to match orders based on price
and generation time if provided.

Previously we've placed request and proposal orders with the same cost,
so these orders should be matched.
Your orders could get matched to different IDs and not the ones you posted,
as it's decided by the matching engine based on several parameters.
For the time being, let's assume the match was for the orders we posted.

To check this, we need to get their statuses once again:

```console
$ python3 scripts/request_tools.py get --key 16153352

{
    "_key": "16153352",
    "proposal_key": "16153923",
    "cost": 2,
    "statement_key": "32326",
    "status": "processing",
}
```

We see that request's `status` changed to `processing`, so the order is matched.
We also see `proposal_key` populated to the matched proposal order.

```console
$ python3 scripts/proposal_tools.py get --key 16153923

Proposal:
 {
    "_key": "16153923",
    "request_key": "16153352",
    "cost": 2,
    "matched_time": 1675938454814,
    "statement_key": "32326",
    "status": "processing",
}
```

We see that proposal's `status` has also changed to `processing`, and `request_key`
was populated to the matched request order.

## Generate a proof

Once the orders are matched, the proof producer should create a proof for the circuit.
They're going to need the following parameters:
  * `circuit_input` — circuit statement from Proof Market;
  * `public_input` — public inputs for the statement used in the request;
  * `proof_out` — location to store the output proof file.

Ideally, you should retrieve both the `circuit_input` and `public_input` from Proof Market.
See the corresponding part
of the [proof producer's guide](../producers/proof-producer.md#check-order-status-and-fetch-inputs).

```console
$ ./build/bin/proof-generator/proof-generator \
    --circuit_input example/statements/arithmetic_example_statement.json \
    --public_input example/input/arithmetic_example/input.json \
    --proof_out arithmetic_proof.bin

Inner verification passed
0
```

If no errors were encountered, you should now have `arithmetic_proof.bin` file
in your directory.

## Publish a proof

You can publish a proof like this:

```console
$ python3 scripts/proof_tools.py push \
    --proposal_key 16153923 \
    --file arithmetic_proof.bin

Proof is pushed
```

If you check the request's or proposal's status once again,
you will see that they now have the proof's key, and order's status changed to `completed`.

```console
$ python3 scripts/request_tools.py get --key 16153352

Requests:
 {
    "_key": "16153352",
    "proposal_key": "16153923",
    "cost": 2,
    "proof_key": "16158141",
    "statement_key": "32326",
    "status": "completed",
    "updatedOn": 1675942301032,
 }
```

To retrieve proof from Proof Market, you'll need either the proof key or the request key.

```bash
python3 scripts/proof_tools.py get \
    --proof_key 16158141 | --request_key 16153352
```

Congratulations!
If you have made it this far, you've performed a full interaction between a proof producer
and a proof requester.
