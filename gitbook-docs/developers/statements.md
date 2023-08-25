# Circuits and statements

Anyone can generate circuits.
Circuits are serialized & published on Proof Market, allowing their reuse
by all market participants.
The most efficient way to create a circuit definition for Proof Market is
[zkLLVM](https://github.com/NilFoundation/zkllvm) circuit compiler.
You can find the required dependencies and build instructions in the
[zkLLVM's repository](https://github.com/NilFoundation/zkllvm).

## Create a circuit

Write a circuit and compile it with [zkLLVM](https://github.com/NilFoundation/zkllvm):

```bash
make -C ${ZKLLVM_BUILD:-build} <circuit target name> -j$(nproc)
```

For more information on circuit development, read the [zkLLVM documentation](
https://docs.nil.foundation/zkllvm/circuit-development/circuit-generation).

## Prepare a statement with a circuit description for Proof Market

Circuits are stored on Proof Market in the form of statements.
Example statements can be found in the `example` directory.

```bash
python3 scripts/prepare_statement.py \
    --circuit <zkllvm output> \
    --type <circuit type> \
    --output <statement description file> \
    --name <statement name> \
    --private | --public
```

## Publish a statement on Proof Market

{% hint style="info" %}
[Sign up or sign in to Proof Market](../market/web-interface.md#creating-an-account)
and keep the authentication files to use the following command line tools.
{% endhint %}

You can find an authentication file example in the `example` directory.

The statement can be published on Proof Market via the Python script `statement_tools.py`:

```console
$ python3 scripts/statement_tools.py push 
    --file <json file with statement description>

Statement from /opt/zkllvm-template/build/template.json was pushed.
```

## Retrieving statements

A list of all available statements can be obtained by this command:

```bash
python3 scripts/statement_tools.py get
```

To retrieve a selected statement/circuit definition and its metadata
uploaded upon the statement's publishing:

```bash
python3 scripts/statement_tools.py get \
    --key <key of the statement> \
    --output <output file> 
```
