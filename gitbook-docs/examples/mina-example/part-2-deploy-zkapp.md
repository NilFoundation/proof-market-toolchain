# Part 2. Deploying zkApp

The following steps, performed from `mina-add-zkapp` home directory,
deploy the test zkApp to the `Berkeley` test net.

## Step 1: generate a key pair

Let's start with generating test keys:

```console
$ npm run build && node build/src/generateKeyPairs.js

--------WARNING: UNSECURE KEYS DO NOT USE IN PRODUCTION ENV----------------

--------------------------------------------------------------------------
zkApp private key: EKxxxxxxxxxxxxxxxxxxxxxxxxx
zkApp public key : B62xxxxxxxxxxxxxxxxxxxxxxxx
--------------------------------------------------------------------------
user private key : EKxxxxxxxxxxxxxxxxxxxxxxxxx
user public key  : B62xxxxxxxxxxxxxxxxxxxxxxxx
--------------------------------------------------------------------------
```

Copy these keys to the corresponding paths:
* `zkApp` key pair to `keys/berkeley.json`;
* `user` key pair to `keys/user.json`.

The format for these JSON files is:
```json
{
    "privateKey": "YOUR_PRIVATE_KEY",
    "publicKey": "YOUR_PUBLIC_KEY"
}
```

On https://faucet.minaprotocol.com you can fund your Mina wallet on `Berkeley` network
by requesting faucet funds using your **public** key.
**Only your wallets** should be funded this way.

{% hint style="warning" %}
You must wait for your wallet to be funded before progressing.
This depends on your transaction appearing in a completed block, which can take approximately 3-5 minutes.
{% endhint %}

## Step 2: build the project

To build the project, run:
```bash
npm run build
```

## Step 3: deploy the project

Upon deployment, zkApp's 8 state variables will be initialized to `1,2,3,4,5,6,7,8`.

```bash
zk deploy berkeley
```

The test `Add` contract has the only method, `update`, that increments the zkApp state fields
by `1,2,3,4,5,6,7,8` each of them respectively.
Note that state values are represented in the hexadecimal numeral system.

## Step 4: check the account state

To fetch zkApp or user data, you're going to need the `get_mina_state.py` script
located in `mina-state-proof/scripts`.
You can either navigate to the `mina-state-proof` home or add `../../` before `scripts/get_mina_state.py`.

```bash
$ python3 scripts/get_mina_state.py \
    --output mina_zkapp_state.json \
    account --address YOUR_PUBLIC_KEY
```

The output file you provided will be populated with JSON
detailing the account/zkApp state:

```json
{
    "public_key": "YOUR_PUBLIC_KEY",
    "balance": {
        "liquid": "0",
        "locked": "0"
    },
    "state":
        "0x0000000000000000000000000000000000000000000000000000000000000001,
        0x0000000000000000000000000000000000000000000000000000000000000002,
        0x0000000000000000000000000000000000000000000000000000000000000003,
        0x0000000000000000000000000000000000000000000000000000000000000004,
        0x0000000000000000000000000000000000000000000000000000000000000005,
        0x0000000000000000000000000000000000000000000000000000000000000006,
        0x0000000000000000000000000000000000000000000000000000000000000007,
        0x0000000000000000000000000000000000000000000000000000000000000008",
    "proof_extension": "..."
}
```

## Step 5: interacting with zkApp

This step is performed from `mina-add-zkapp` home.
It will compile the contract and call its method `update`.
This may take some time, but if successful you will see a link to your transaction on `Berkeley`:

```console
$ node build/src/interact.js berkeley

compile the contract...
build transaction and create proof...
send transaction...

Success! Update transaction sent.

Your smart contract state will be updated
as soon as the transaction is included in a block:
https://berkeley.minaexplorer.com/transaction/...
```

Every interaction through this script calls the contract's `update` method,
incrementing the values as follows:
* after the first update state values will be changed to `2,4,6,8,a,c,e,10`;
* after the second update state values will be changed to `3,6,9,c,f,12,15,18`;
* and so on.

Once your transaction is included in a block, in approximately 3-5 minutes,
you can [check the state values](#step-4-check-your-state-values) again and see if they changed.
