# Proof Requester

## Submit Bid Order

The proof requester can create an order. An order has additional details such as who is requesting the proof and what they are willing to pay for it

```
python3 scripts/bid_tools.py push --cost <cost of the bid> --file <json file with public_input> --key <key of the statement> 
```

## Order Status

The proof requester can check their bid with

```
python3 scripts/bid_tools.py get --key <key of the bid> 
```

## Get Proof

Now the proof requester is able to get their proof either by bid key or proof key.

```
python3 scripts/proof_tools.py get --bid_key <key of the bid> 
```
