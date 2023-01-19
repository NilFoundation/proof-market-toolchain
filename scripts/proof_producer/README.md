# Proof Producer Daemon

Here is PoC for Proof Producer daemon. 
The main purpose of it is to authomatization of the Proof Producer routine in creating asks and generating proofs. 
Please, get familiar with [step-by-step](../../README.md) guide firstly. 

## Prepare Statements 
Firstly, choose statements what are you interested in.
You can find all statement with our [web interface](proof.market) or using `statement_tools`. 

Add the statements' keys to `MY_STATEMENTS` from `constants.py`. 

Run
```
python3 proof_producer.py --prepare_statements -d <statements dierctory>
```

