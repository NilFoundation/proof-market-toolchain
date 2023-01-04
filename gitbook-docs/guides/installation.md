# Installation

{% hint style="info" %}
Please ensure you have done the [environment](environment-setup.md) setup before progressing.
{% endhint %}

The proof market tool chain is a collection of python scripts which allows users to interact with the proof market. These set of scripts are required for all participants.

Clone the repository

```shell
git clone --recurse-submodules git@github.com:NilFoundation/proof-market-toolchain.git
```

Based on the users role ,they need to install additional binaries/scripts to interact with the market.

## Circuit Developer

Please follow the installation guide for :

* [zkLLVM](https://docs.nil.foundation/zkllvm/guides/installation)&#x20;
  * &#x20;Used to compile your circuit and generate a binary to be hosted on proof market

## Proof Requester

* Validation artifacts
  * These could be smart contracts or zkLLVM for local validation.

## Proof Generator

* [zkLLVM](https://docs.nil.foundation/zkllvm/guides/installation)&#x20;
  * Used to create proofs for the circuits
  * Validate proofs on client side&#x20;





###

