---
description: Managing proof request orders
---

# Proof request utilities

This page describes the script with utilities for proof requesters — `request_tools.py`.

Make sure you've [signed up](user.md) before continuing.

To see all parameters that can be passed to the script, run it with the `-h` option:

```console
$ python3 request_tools.py -h

usage: request_tools.py [-h] [--auth AUTH] [-v] {push,get} ...

positional arguments:
  {push,get}     sub-command help
    push         push request
    get          get request

options:
  -h, --help     show this help message and exit
  --auth AUTH    auth file
  -v, --verbose  increase output verbosity
```

## Submitting a proof request
### Usage

```console
$ python3 request_tools.py push -h

usage: request_tools.py push [-h] --cost COST --file FILE --key KEY \
       [--generation_time GENERATION_TIME]

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
$ python3 request_tools.py push --cost=70 --key=969894

{
   "_key":"15970719",
   "_id":"request/15970719",
   "_rev":"_fgwuuI----",
   "statement_key":"969894",
   "cost":70,
   "sender":"helloworld",
   "wait_period":"None",
   "eval_time":"None",
   "createdOn":1675787026590,
   "updatedOn":1675787026591,
   "status":"created"
}
```

## Retrieving info on proof request
### Usage

Proof requesters can check their requests with the following command:
 
```console
$ python3 request_tools.py get -h

usage: request_tools.py get [-h] [--key KEY] [--request_status REQUEST_STATUS]

options:
  -h, --help            show this help message and exit
  --key KEY             request key
  --request_status REQUEST_STATUS
                        request status
```

### Example

```console
$ python3 request_tools.py get --key=15970719

 {
    "_key": "15970719",
    "cost": 70,
    "createdOn": 1675787026590,
    "eval_time": null,
    "sender": "helloworld",
    "statement_key": "969894",
    "status": "created",
    "updatedOn": 1675787026591,
    "wait_period": null
}
```
