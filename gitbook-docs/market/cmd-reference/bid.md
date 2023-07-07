---
description: Managing proof proposals and retrieving public inputs
---

# Proposal utilities

`=nil;` prepared a [toolchain](https://github.com/NilFoundation/proof-market-toolchain)
for interacting with the Proof Market.
This page describes two scripts with utilities for proof producers,
`proposal_tools.py` and `public_input_get.py`.

Make sure you've [signed in or signed up](user.md) before continuing, and that you're registered
as a proof producer.

## Usage

To see all parameters that can be passed to the script, run it with the `-h` option:

```console
foo@bar:~$ python3 proposal_tools.py -h

usage: proposal_tools.py [-h] [--auth AUTH] {push,get} ...

positional arguments:
  {push,get}   sub-command help
    push       push proposal
    get        get proposal

options:
  -h, --help   show this help message and exit
  --auth AUTH  auth file
```

## Submitting proposal

```console
foo@bar:~$ python3 proposal_tools.py push -h

usage: proposal_tools.py push [-h] --cost COST --file FILE --key KEY [--generation_time GENERATION_TIME]

options:
  -h, --help            show this help message and exit
  --cost COST           cost
  --file FILE           json file with public input
  --key KEY             statement key
  --generation_time GENERATION_TIME
                        required proof time generation (in mins)
```

### Example

```console
foo@bar:~$ python3 proposal_tools.py push --cost=5 --file=public_inputs.json --key=32292

"Limit proposal":{
   "_key":"15970220",
   "_id":"proposal/15970220",
   "_rev":"_fgwlYTK---",
   "statement_key":"32292",
   "input":{
      "data":{
      .....
   },
   "cost":5,
   "sender":"helloworld",
   "wait_period":"None",
   "eval_time":"None",
   "createdOn":1675786414203,
   "updatedOn":1675786414203,
   "status":"created"
}
```

## Retrieving proposal

Proof producers can check their proposals like his:

```console
foo@bar:~$ python3 proposal_tools.py get -h

usage: proposal_tools.py get [-h] [--key KEY] [--proposal_status PROPOSAL_STATUS]

options:
  -h, --help            show this help message and exit
  --key KEY             proposal key
  --proposal_status PROPOSAL_STATUS
                        proposal status
```

### Example

```console
foo@bar:~$ python3 proposal_tools.py get --key=15077121

"proposals":{
   "_key":"15077121",
   "cost":5,
   "createdOn":1675377961857,
   "eval_time":null,
   "input":{
      "data":{
		...
   },
   "sender":"helloworld",
   "statement_key":"32292",
   "status":"created",
   "updatedOn":1675377961857,
   "wait_period":null
}
```

# Request's public inputs

To generate a proof, the proof producer will need statement's definition and public inputs.

## Usage

You can retrieve public inputs for a proposal like this:

```console
foo@bar:~$ python3 public_input_get.py -h

usage: public_input_get.py [-h] [-k key] [--auth auth] -o file

options:
  -h, --help            show this help message and exit
  -k key, --key key     key of the request
  --auth auth           auth
  -o file, --output file
                        output file
```

## Example

```bash
python3 public_input_get.py --key=15077121 -o=tmp_input.json
```
