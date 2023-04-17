---
description: Scripts to manage bid orders
---

# bid

## Submit bid

```
usage: bid_tools.py push [-h] --cost COST --file FILE --key KEY [--generation_time GENERATION_TIME]

options:
  -h, --help            show this help message and exit
  --cost COST           cost
  --file FILE           json file with public input
  --key KEY             statement key
  --generation_time GENERATION_TIME
                        required proof time generation (in mins)
```

Ex:

```
python3 bid_tools.py push --cost=5 --file=public_inputs.json --key=32292
```

Output

```
"Limit bid":{
   "_key":"15970220",
   "_id":"bid/15970220",
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

## Retrieve bid

```
usage: bid_tools.py get [-h] [--key KEY] [--bid_status BID_STATUS]

options:
  -h, --help            show this help message and exit
  --key KEY             bid key
  --bid_status BID_STATUS
                        bid status

```

ex:

```
python3 bid_tools.py get --key=15077121
```

Output

```
"Bids":{
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

## Retrieve Public inputs

Proof generators can retrieve and use public inputs for a bid order.&#x20;

```
python3 public_input_get.py --h
usage: public_input_get.py [-h] [-k key] [--auth auth] -o file

options:
  -h, --help            show this help message and exit
  -k key, --key key     key of the bid
  --auth auth           auth
  -o file, --output file
                        output file

```

ex:

```
python3 public_input_get.py --key=15077121 -o=tmp_input.json
```

