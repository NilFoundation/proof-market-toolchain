# Proof producer

{% hint style="info" %}
[Sign up](sign-up.md) and keep the authentication files in order to use
the following command line tools.
{% endhint %}

## Submit a proposal

Proof producers can submit proposals for statements and specify an accompanying cost like this:

```bash
python3 scripts/proposal_tools.py push \
    --cost <cost of the proposal> \
    --key <key of the statement>
```

## Check order status and fetch inputs

Producers can check their proposal using `proposal_tools`:

```bash
python3 scripts/proposal_tools.py get --key <key of the proposal>
```

If the proposal's status is `processing`, it means that the proposal was matched with the request.
Now it's time for the proof producer to generate the proof.

First of all, the proof producer needs statement definition:

```bash
python3 scripts/statement_tools.py get \
    --key <key of the statement>\
    --output <output file>
```

Next, public input of the request:

```bash
python3 scripts/public_input_get.py \
    --key <request key> \
    --output <output file path>
```

## Generate a proof 

To generate proof execute the following:

```bash
cd build
./bin/proof-generator/proof-generator \
    --proof_out <output file> \
    --circuit_input <statement from Proof Market>
    --public_input <public input from Proof Market>
```

## Submit a proof

The proof producer can now submit the proof to the marketplace, where they will get the reward
if the proof is verified by Proof Market.
You can learn more about
[rewards and commissions](../economics.md#funds-transferring-and-commissions), as well as about
[penalties](../economics.md#penalties), on the [Economics page](../economics.md).

Submitting a proof via `proof_tools.py`:

```bash
python3 scripts/proof_tools.py push \
    --request_key <key of the request> | --proposal_key <key of the proposal> \
    --file <file with the proof>
```

Note that you can provide only one of the two possible keys.
