# Proof Producer

{% hint style="info" %}
Ensure you have done [Authentication](sign-up.md) setup before progressing
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

Execute the below to generate proof:

```

cd build
./bin/proof-generator/proof-generator --proof_out=<output file> --circuit_input=<statement from Proof Market> --public_input=<public input from Proof Market>
# Or using the multithreaded version
./bin/proof-generator/proof-generator-mt --proof_out=<output file> --circuit_input=<statement from Proof Market> --public_input=<public input from Proof Market> --smp=<number of threads> ----shard0-mem-scale=<scale>

```

For multithreaded versions, the following flags warrant discussion as the computation of proof requires temporary space. This is always allocated to `core0`/`shard0.`

* `smp` : Number of threads. This is ideally the number of CPU cores on the system. ex: If you have a 16 core CPU &  16 GB RAM & set `smp` to 16. Each core gets access to 1 GB of RAM. Thus `core0`/`shard0` will have access to 1 GB of RAM.
* `shard0-mem-scale`: Weighted parameter (`weight`) to reserve memory for `shard0.`This is a natural number which allows for `shard0` to have more access to RAM in comparison to others. ex: If you have a 16 core CPU &  16 GB RAM & set `smp` to 8 and set `shard0-mem-scale` to 9.

&#x20;We first compute  `ram_per_shard` variable as follows:

```
ram_per_shard = TOTAL_RAM / (smp + shard0-mem-scale -1)
              = 16 / (8 + 8)
              = 1
```

This equates to:&#x20;

* `shard0` RAM =`shard0-mem-scale` = 9 GB
* `shard1` .... `shard7` = `ram_per_shard` = 1 GB/per core= 7 GB

## Proof Submission

The proof generator can now submit the proof to the marketplace, where, they will get the reward if verified.

```
python3 scripts/proof_tools.py push --bid_key <key of the bid> --ask_key <key of the ask> --file <file with the proof> 
```

You can provide only one of two possible keys.&#x20;
