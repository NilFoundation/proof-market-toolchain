# Proof Market Endpoint for Ethereum

Proof Market EVM Endpoint is a solution that enhances the capabilities of [Proof Market](https://proof.market) by enabling direct proof ordering for Ethereum applications.
This extension allows for trustless, provable computation composability for all EVM applications.
The Ethereum Endpoint acts as a gateway interface facilitating communication between EVM applications and Proof Market.
It enables several crucial operations:
* Requesting zkProofs: EVM applications can directly request zero-knowledge proofs for specific computations.
* Payment Facilitation: Payment for proof producers can be facilitated directly through Ethereum, streamlining the payment process.
* Proof Verification: The Ethereum Endpoint acts as an additional verifier in the proof generation pipeline, ensuring the validity of incoming proofs.

It's important to note that the Ethereum Endpoint only stores verification keys for the statements that require proof.
Due to Ethereum's limitations in storing large data chunks, this approach proves to be more economically advantageous.
Application developers should note that they need a one-time setup to upload their circuit to the original Proof Market.
Currently, Proof Market works with the Placeholder Proof System, but support for other proof systems can be requested.

## Contract Addresses
### Endpoint

| Network      | Address |
| ----------- | ----------- |
| Sepolia      | [`0xD8EC705993EfFF8512de7FD91d079375b9589C90`](https://sepolia.etherscan.io/address/0xD8EC705993EfFF8512de7FD91d079375b9589C90)       |

### UnifiedAdditionVerifier

| Network      | Address |
| ----------- | ----------- |
| Sepolia      | [`0xc6E7A6Dcad73D499520DDdf5d9b56E0E18DD9bAd`](https://sepolia.etherscan.io/address/0xc6E7A6Dcad73D499520DDdf5d9b56E0E18DD9bAd)       |

## Cloning the repository

Clone the project from GitHub:

```bash
git clone git@github.com:NilFoundation/evm-proof-market.git
```

After that, navigate to the `evm-proof-market` directory:

```bash
cd evm-proof-market
```

## Installation

```bash
npm install
```

## Commands
### Compile

Compile the contracts to ensure ABI files are present:

```bash
npx hardhat compile
```

### Test

Execute tests for the contracts:

```bash
npx hardhat test #Execute tests
```

### Deploy

Deploy the contracts to a specified network:

```bash
npx hardhat deployContract --network <network>
```

To deploy to a local network:

```bash
npx hardhat node
```

And then deploy the contract to the local network in a separate terminal:

```bash
npx hardhat deployContract --network localhost
```

### Update

Check storage layout of the contracts:

```bash
npx hardhat check
```

The following requirements must be met:
- Existing layout of storage slots must be preserved
(except for the `gap` arrays)
- Any new storage slots must be added `at the end` of the contract
- Length of the gap arrays must be decreased so the storage layout is preserved

### Usage

Get a list of the available commands:

```bash
node scripts/interact.js -h
```

### Flags

    `providerUrl`: URL of the Ethereum provider.
    If not specified, it defaults to 'http://localhost:8545'.
    `statementId`: ID for creating a new order or fetching a statement's price.
    `price`: Price of the order when creating a new order.
    `inputFile`: JSON file path suitable for the statement format when creating a new order.
    `keystoreFile`: Path of the keystore file containing the private key
    for signing transactions.
    If not specified, it defaults to `keystore.json`.
    `password`: Password for the keystore file.
    `verifiers`: The verifiers of a statement as a comma-separated list.

### Usage
#### Creating a keystore file from a private key

```bash
node scripts/interact.js createKeystoreFromPrivateKey \
    --pk <privateKey> \
    --password <password>
```

This command will create a keystore file from a private key and save it
in the `keystore.json` file.
Later, this file can be used to sign transactions by specifying
the `keystoreFile` and `password` flags.

#### Mint and approve tokens

```bash
node scripts/interact.js mintAndApprove \
    --password <password> \
    --keystoreFile <keystoreFile> \
    --providerUrl <providerUrl>
```

This command will mint sufficient for testing ERC20 tokens and approve
the smart contracts to spend them.
Note that on a main net deployment we will use one of the existing
standard ERC20 tokens, like USDT.

#### Getting the price for a statement

```bash
node scripts/interact.js getPrice \
    --statementId <statementId> \
    --keystoreFile <keystoreFile> \
    --providerUrl <providerUrl>
```

#### Creating a new order

```bash
node scripts/interact.js createOrder \
    --statementId <statementId> \
    --price <price> \
    --inputFile <inputFilePath> \
    --password <password> \
    --keystoreFile <keystoreFile> \
    --providerUrl <providerUrl>
```

## Testing usage on a local network

1. Start a local hardhat network from the `mina-state-proof` repository:

```bash
npm i
npx hardhat node
```

2. Deploy the contract and add statements:

```bash
npx hardhat run scripts/deploy.js --network localhost
```

It will create a `deployed_addresses.json` file with the addresses
of the deployed contracts.

Note:
- UnifiedAddition verifier has to be deployed manually (ignore for now)
- Mina verifiers have to be automatically deployed

On local hardhat node addresses are persistent, so you can just run
the following command to add statements:

```bash
npx hardhat run scripts/addStatements.js --network localhost
```

3. Obtain private key of some account from the local network
(can be obtained from the console output of the first command)

4. Create a keystore file from the private key:

```bash
node scripts/interact.js createKeystoreFromPrivateKey \
    --pk <privateKey> \
    --password <password>
```

5. Mint and approve tokens:

```bash
node scripts/interact.js mintAndApprove \
    --password <password> \
    --keystoreFile <keystoreFile>
```

6. Create a new order:

```bash
node scripts/interact.js createOrder \
    --statementId <statementId> \
    --price <price> \
    --inputFile <inputFilePath> \
    --password <password> \
    --keystoreFile <keystoreFile>
```

For example, for Mina account statement

```bash
node scripts/interact.js createOrder \
    --statementId 79169223 \
    --price 17 \
    --inputFile scripts/test_inputs/account_mina.json \
    --password <password>
```

## Maintenance

Contract maintenance is facilitated by Hardhat tasks located in `scripts/maintain.js`.
Available tasks are `deployContract`, `upgradeContract`, `addStatements`,
and `updateStatementVerifiers`.
Execute a task with the following:

```bash
npx hardhat <taskName> --network <network>
```
