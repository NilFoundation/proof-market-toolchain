# Proof Producer

{% hint style="info" %}
Ensure you have done [Authentication](authentication.md) setup before progressing
{% endhint %}

## Submit Ask order

The Proof producers can submit asks for circuits and specify an accompanying cost.

```
python3 scripts/ask_tools.py push --cost <cost of the ask> --key <key of the statement> 
```

## Order Status/Fetch Inputs

The proof producer can check their ask with

```
python3 scripts/ask_tools.py get --key <key of the ask> 
```

Ask's status 'processing' means that the ask was matched with a bid. Now it is time to generate a proof for the proof producer.

First of all, the proof producer needs circuit definition:

```

python3 scripts/statement_tools.py get --key <key of the statement> -o <output file> 

```

Next, public input of the bid:

```
python3 scripts/public_input_get.py --key <bid key> -o <output file path> 
```

## Proof Generation

Execute the below to generate a proof:

```

cd build
./bin/proof-generator/proof-generator --proof_out=<output file> --circuit_input=<statement from Proof Market> --public_input=<public input from Proof Market>
```

## Proof Submission

The proof generator can now submit the proof to the marketplace, where if verified, they will get the reward.

```
python3 scripts/proof_tools.py push --bid_key <key of the bid> --ask_key <key of the ask> --file <file with the proof> 
```

You can provide only one of two possible keys.&#x20;
