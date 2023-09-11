# Part 4. Proof validation

These steps are performed from the `mina-state-proof` home directory.

## Step 1: collecting the data

If everything went well, you now have two files with proofs,
and all that's left to do is to verify them.
For that you'll need:

* For ledger state proof
    * file with the ledger proof from Proof Market;
    * ledger hash which this proof attests to.
* For account state proof
    * file with the account proof from Proof Market;
    * ledger hash which this proof attests to (same as for ledger state proof);
    * account state data (in JSON).

## Step 2: launch a Hardhat node

Execute the following to launch a Hardhat node:

```console
$ npx hardhat node

Started HTTP and WebSocket JSON-RPC server at http://127.0.0.1:8545/

Accounts
========
Account #0: 0xf39Fd6e51aad88F6F4ce6aB8827279cffFb92266 (10000 ETH)
Private Key: 0xac0974bec39a17e36ba4a6b4d238ff944bacb478cbed5efcae784d7bf4f2ff80
...

WARNING: These accounts, and their private keys, are publicly known.
Any funds sent to them on Mainnet or any other live network WILL BE LOST.
```

Don't close the terminal and don't finish this process, the Hardhat node should be
running for the next steps.

## Step 3: deploy contracts

From another terminal, execute the following in the `mina-state-proof` home:

```console
$ npx hardhat deploy --network localhost

Nothing to compile
No need to generate any newer typings.
deploying "mina_base_gate0" (tx: <ID>)...: deployed at <ID> with x gas
...
deploying "MinaState" (tx: <ID>)...: deployed at <ID> with x gas
```

Hardhat reuses old deployments; to force re-deploy, add the `--reset` flag to the command.

## Step 4: validation

Once the contracts are deployed, we can use them to validate proofs.
Below are two Hardhat proposals that execute flows to validate the ledger and account states.
Note these work against the above deployment, so you must run the deployment
and wait for it to finish before continuing.

### Validating the ledger state

Insert the ledger hash and the path to the mina ledger state proof to the following command:

```bash
npx hardhat validate_ledger_state \
    --proof ../proof-market-toolchain/ledger_proof.bin \
    --ledger LEDHER_HASH \
    --network localhost
```

For example, there's a test ledger state in the repo, you can validate it:

```console
$ npx hardhat validate_ledger_state \
    --proof ./test/data/proof_state.bin \
    --ledger jwYPLbRQa4X86tSJs1aTzusf3TNdVTj58oyWJQB132sEGUtKHcB \
    --network localhost

jwYPLbRQa4X86tSJs1aTzusf3TNdVTj58oyWJQB132sEGUtKHcB
{
  ...
  events: [
    {
      event: 'LedgerProofValidated',
      eventSignature: 'LedgerProofValidated()',
      ...
    },
    {
      event: 'LedgerProofValidatedAndUpdated',
      eventSignature: 'LedgerProofValidatedAndUpdated()',
      ...
    }
  ]
}
```

### Validating account state

Upon [checking the account state](part-2-deploy-zkapp.md#step-4-check-the-account-state),
it was saved to `mina_zkapp_state.json` in the `mina-state-proof` home.
There's also an example of account data at `examples/data/account_data.json`.
Basically, it should look like this:

```json
{
    "public_key":YOUR_PUBLIC_KEY,
    "balance": {
        "liquid": Unlocked balance in MINA,
        "locked": Locked/Staked balance in MINA
    },
    "state": 8-byte state of zk app or user account,
    "proof_extension": proof id
}
```

The `state` needs to be converted into a padded comma-separated string.

To see validation of account bytes in Ethereum, replace the following parameters:
* proof — file path to the account state proof that is validated;
* ledger — hash of the ledger against which the account state is validated.
  This should be the hash that has already been committed in the previous step;
* state — file path to the account state to which the above proof attests.

```bash
npx hardhat validate_account_state \
    --proof ../proof-market-toolchain/account_proof.bin \
    --state ./mina_zkapp_state.json \
    --ledger LEDGER_HASH \
    --network localhost
```

For example, there's a test account state in the repo:

```bash
npx hardhat validate_account_state \
    --proof ./test/data/proof_account.bin \
    --state ./examples/data/account_data.json \
    --ledger jwYPLbRQa4X86tSJs1aTzusf3TNdVTj58oyWJQB132sEGUtKHcB \
    --network localhost
```

Congratulations! If you have reached here, you have now managed to validate both ledger
and account proofs in Ethereum.
