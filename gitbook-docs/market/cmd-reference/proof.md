---
description: Scripts to manage submission/retrieval of proofs
---

# proof

## Submit Proof

```
python proof_tools.py push --h
usage: proof_tools.py push [-h] [-a ASK_KEY] [-b BID_KEY] -f FILE

options:
  -h, --help            show this help message and exit
  -a ASK_KEY, --ask_key ASK_KEY
                        ask_key
  -b BID_KEY, --bid_key BID_KEY
                        bid_key
  -f FILE, --file FILE  file with proof

```

Ex:

```
python3 proof_tools.py push --bid_key=458426 --file=proof.bin
```

Output:

```
Proof for 458426 is pushed
```

## Retrieve Proof

```
python proof_tools.py get --h 
usage: proof_tools.py get [-h] [-p PROOF_KEY] [-f FILE] [-b BID_KEY]

options:
  -h, --help            show this help message and exit
  -p PROOF_KEY, --proof_key PROOF_KEY
                        key of the proof
  -f FILE, --file FILE  file to write proof
  -b BID_KEY, --bid_key BID_KEY
                        bid_key

```

Ex:

```
python3 proof_tools.py get -f tmp_proof.bin -b 458426 
```

Output

```
N/A
```
