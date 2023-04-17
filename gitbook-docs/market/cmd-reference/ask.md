---
description: Scripts to manage ask orders
---

# ask

Usage

```
python3 ask_tools.py --h    
usage: ask_tools.py [-h] [--auth AUTH] {push,get} ...

positional arguments:
  {push,get}   sub-command help
    push       push ask
    get        get ask

options:
  -h, --help   show this help message and exit
  --auth AUTH  auth file


```



## Submit ask

```
python3 ask_tools.py push --h                   
usage: ask_tools.py push [-h] --cost COST --key KEY

options:
  -h, --help   show this help message and exit
  --cost COST  cost
  --key KEY    key of the statement

```

Ex:

```
python3 ask_tools.py push --cost=70 --key=969894
```

Output

```
{
   "_key":"15970719",
   "_id":"ask/15970719",
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

## Retrieve Ask

```
python3 ask_tools.py get --h
usage: ask_tools.py get [-h] [--key KEY]

options:
  -h, --help  show this help message and exit
  --key KEY   key of the ask

```

Ex:

```
python3 ask_tools.py get --key=15970719
```

Output

```
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
