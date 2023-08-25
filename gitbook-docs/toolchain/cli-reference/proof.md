---
description: Managing submission and retrieval of proofs
---

# Proof utilities

This page describes the script with proof utilities — `proof_tools.py`.

Make sure you've [signed up](user.md) before continuing.

To see all parameters that can be passed to the script, run it with the `-h` option:

```console
$ python3 proof_tools.py -h
usage: proof_tools.py [-h] [--auth AUTH] {push,get} ...

positional arguments:
  {push,get}   sub-command help
    push       push proof
    get        get proof

options:
  -h, --help   show this help message and exit
  --auth AUTH  auth
```

## Submitting proof

You'll need the proof file and request or proposal key to submit proof to Proof Market.
You can provide only one of the two possible keys.

### Usage

```console
$ python3 proof_tools.py push -h

usage: proof_tools.py push [-h] [-p PROPOSAL_KEY] [-r REQUEST_KEY] -f FILE

options:
  -h, --help            show this help message and exit
  -p PROPOSAL_KEY, --proposal_key PROPOSAL_KEY
                        proposal_key
  -r REQUEST_KEY, --request_key REQUEST_KEY
                        request_key
  -f FILE, --file FILE  file with proof
```

### Example

```console
$ python3 proof_tools.py push --proposal_key=458426 --file=proof.bin

Proof for 458426 is pushed
```

## Retrieving proof

To retrieve proof from Proof Market, you'll need either the proof key or the request key.

### Usage

```console
$ python3 proof_tools.py get -h 

usage: proof_tools.py get [-h] [-k PROOF_KEY] [-f FILE] [-r REQUEST_KEY]

options:
  -h, --help            show this help message and exit
  -k PROOF_KEY, --proof_key PROOF_KEY
                        key of the proof
  -f FILE, --file FILE  file to write proof
  -r REQUEST_KEY, --request_key REQUEST_KEY
                        request_key
```

### Example

```bash
python3 proof_tools.py get -f tmp_proof.bin -r 458426 
```
