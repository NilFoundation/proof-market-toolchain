# Circuit Developer

This is an entity that prepares circuits for the proof market. The most efficient way to create a circuit definition for Proof Market is [zkLLVM](https://github.com/NilFoundation/zkllvm). All dependencies and build instructions are inside the [zkLLVM's repository](https://github.com/NilFoundation/zkllvm).

Anyone can generate circuits. They are serialised & published on the proof market. This allows the reuse of the circuits by all other proof requesters.&#x20;

## Create a new circuit (using [zkLLVM](https://github.com/NilFoundation/zkllvm))

```
make -C ${ZKLLVM_BUILD:-build} <circuit target name> -j$(nproc)
```

## Prepare a statement with a circuit description for Proof Market

Circuits are stored as a _statement_ structure on Proof Market. Statement description example can be found in /example directory

```
python3 scripts/prepare_statement.py -c <zkllvm output> \
-o <statement description file> \ 
-n <statement name> -t <circuit type>
```

Provide the necessary information listed in the output statement file

## Publish to Proof Market

{% hint style="info" %}
Ensure you have done the [Authentication](sign-up.md) setup before progressing
{% endhint %}



This statement can now be pushed to the Proof market via the Python script

```
python3 scripts/statement_tools.py push --file <json file with statement description> 
```

Authorisation file examples can be found in `/example` directory.

You will be returned an object containing a _\_key_ filed -- unique descriptor of the statement

## Published circuit id

A list of all available statements can be obtained by

```
python3 scripts/statement_tools.py get
```

\
