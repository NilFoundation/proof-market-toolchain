# Part 3. Requesting a proof

In this part, we will collect the information required to submit a proof request
to be validated on EVM and post such a request.

## Step 1: collecting the data

For proof validation in the smart contract, we're going to need Mina's ledger hash and our account state.
Navigate to the `mina-state-proof` home directory.

### Get ledger state

First, let's get the ledger data — the entire Mina network state:

```console
$ python3 scripts/get_mina_state.py \
    --output mina_ledger_state.json \
    ledger

Fetching data for block height: 5612
Hash: jwkscPfXvGVVvgm92wroiXBJXE8bRt18S2ue1Uk9k5awGSFCfSM
```

Both the output of the command and the output file contain the `Hash`/`snarkedLedgerHash` attribute
that identifies the ledger:

```json
      "blockchainState": {
        ...
        "snarkedLedgerHash": "jwkscPfXvGVVvgm92wroiXBJXE8bRt18S2ue1Uk9k5awGSFCfSM",
      }
```

### Get account state

[Get the current zkApp state](part-2-deploy-zkapp.md#step-4-check-the-account-state).
Also note, that the ledger state might change while you do that.
We recommend obtaining the ledger state once again,
and if you see that the hash in the output has changed,
you might need to obtain the ledger state and the account state once again. 

## Step 2: send proof requests

This step and the further ones are performed from the `proof-market-toolchain` home.

You can get all statements published on Proof Market like this:

```console
$ python3 scripts/statement_tools.py get

{
    ...
    "_key": "32292",
     "description": "mina state proof",
     "url": "https://github.com/NilFoundation/mina-state-proof"
},
{
    ...
    "_key": "79169223",
    "description": "Account state proof for Mina",
    "url": "https://github.com/NilFoundation/mina-state-proof"
}
```

Look for the following keys in the statement list:
* `_key` **32292** is the key for Mina ledger proof statement (circuit)
  which we'll use for Ledger state proof;
* `_key` **79169223** is the key for Mina account state proof.

That's the two proofs we want to be validated in-EVM.
For that, we're going to use the [`request_tools.py` script](../../toolchain/cli-reference/request.md)
from Proof Market toolchain:

```bash
python3 scripts/request_tools.py push \
    --cost <cost of the request> \
    --file <file scr/path> \
    --key <key of the statement> 
```

Visit the market's web interface at [proof.market](https://proof.market/) to check out
current prices, as they may change based on demand.

### Request a ledger state proof

This proof should validate the full ledger state:

```bash
python3 scripts/request_tools.py push \
    --cost 16 \
    --file ../mina-state-proof/mina_ledger_state.json \
    --key 32292
```

This command will return request's data in JSON format:

```json
{
   "_key":"65017881",
   "statement_key":"32292",
   "status" : "created"
    ...
}
```

We will use the `_key` of that request to check the order status and retrieve the proof.
The key of this order is `65017881`, and the status for now is `created`.

### Request an account state proof

This proof validates the Merkle path of zkApp's state to the ledger state hash:

```console
$ python3 scripts/request_tools.py push \
    --cost 16 \
    --file ../mina-state-proof/mina_zkapp_state.json \
    --key 79169223

{
   "_key":"727121",
   "statement_key":"79169223",
   "status": "created"
    ...
}
```

The key of this order is `727121`, and the status is also `created`.

## Step 3: obtaining the proofs

You must wait until your order status changes from `created` to `completed`.
You can learn more about orders' statuses on the [Economics page](../../market/economics.md#orders-status).

You can check the order status through the [market's web interface](https://proof.market/)
or via CLI like this:

```bash
python3 scripts/request_tools.py get --key <key of the order>
```

### Get the ledger state proof

Once your order status goes to `completed`, you can retrieve the proof.
You're going to need the `proof_key` from the response:

```console
$ python3 scripts/request_tools.py get --key 6501788

{
    "proof_key": 44958982,
    "status": "completed"
}
```

With this `proof_key`, you can get the ledger proof from the market like this:

```console
$ python3 scripts/proof_tools.py get \
    --proof_key 44958982 \
    --file ledger_proof.bin

Proof is saved to ledger_proof.bin
```

### Get the account state proof

Same as above, once the order status is `completed`, we can fetch the account state proof:

```console
$ python3 scripts/proof_tools.py get \
    --proof_key 542042 \
    --file account_proof.bin

Proof is saved to account_proof.bin
```
