# Proof Producer Daemon

Here is PoC for the Proof Producer daemon. 
Its primary purpose is to automate the Proof Producer routine in creating asks and generating proofs. 
Please, get familiar with the [step-by-step](../../README.md) guide firstly. 

## Setup user
Prepare `.user` and `.secret` files in the same way as in the [step-by-step](../../README.md) guide.

## Prepare Statements 
Firstly, choose statements about what you are interested in.
You can find all statements with our [web interface](proof.market) or using `statement_tools`. 

Add the statements' keys to `MY_STATEMENTS` from `constants.py`.

Prepare a directory for statements `<statements_dir>`.

Run
```
python3 proof_producer.py prepare -d <statements_dir>
```

## Start Proof Producer

```
python3 proof_producer.py start -p <path to proof-generator binary>
```