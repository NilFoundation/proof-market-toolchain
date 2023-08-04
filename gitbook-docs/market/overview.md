# Terminology

{% hint style="warning" %}
Proof Market is currently in beta version.
We expect there will be continuous changes in it following the feedback we receive from the customers.
{% endhint %}

## Spot vs. futures

Every marketplace has a set of instruments.
The market has buyers and sellers for each of these instruments.
For example, London Stock Exchange allows users to buy and sell stocks of companies, Binance allows users
to buy and sell Bitcoins — these are known as spot markets because you own the commodity.

Exchanges also allow buying and selling a future price of the instrument, like Binance enables users
to buy and sell future values of Bitcoin via perpetual contracts.
These are called derivative markets, as you do not own the underlying instrument
but are just speculating on its price.

`=nil;` Proof Market belongs to the class of spot markets and is designed
as a decentralized and distributed system.

## Circuits and statements

Markets tend to arrive at a fair price through interactions between buyers and sellers.
In Proof Market, the object of trade are statements — circuits 
prepared for publishing on the market.
Circuits are algebraic representations of an algorithm, created by the proof system
and compiled with the zkLLVM compiler.
For example, it can be MINA or Solana state-proofs.
To publish a circuit to Proof Market, you should build a statement by attaching
all additional metadata to the circuit.
See the [prepare statement](cmd-reference/statement.md/#prepare-statement)
section for more information.

{% hint style="info" %}
To be traded on Proof Market, the statements will require approval by `=nil;` Foundation.
{% endhint %}

## Entities

The marketplace consists of the following entities:

* **Proof requester** — an application like a bridge requesting balance or a user
    interested in a cross-cluster operation or trustless data access.
* **Proof producer** — an entity that will generate the proofs for the requests/orders
    made by the proof requester.

## Orders types

* **Buy order**: A buy order is posted by a proof requester.
    Each buy order comprises the following parameters:
  * **Statement key** — the key of the statement for which this order is placed,
    like MINA or Solana state-proof circuit.
  * **Cost** (in USD) — the amount that the proof requester is willing to pay for the proof.
  * **Order timeout** (in minutes) — how long an order stays in the order book
    once it enters the marketplace.
    The order will be purged if it times out before being matched.
  * **Generation time** (in minutes, optional) — the maximum time the proof producer
    has to publish the proof once the order is matched.

* **Sell order**: A sell order is posted by a proof producer.
    Each sell order has the following parameters:
  * **Statement key** — the key of the statement for which this order is placed,
    like MINA or Solana state-proof circuit.
  * **Cost** (in USD) — the amount that the proof producer is willing to create a proof for.
  * **Order timeout** (in minutes) — how long an order stays in the order book
    once it enters the marketplace.
    The order will be purged if it times out before being matched.
  * **Generation time** (in minutes, optional) — the maximum time the proof producer
    has to publish the proof once the order is matched.

## Order matching

In this beta version of Proof Market, a matching engine matches the orders
when the following parameters are met:

* cost;
* order timeout;
* generation time.

## Order settlement

Order settlement takes place once the proof producer has submitted the proof to the marketplace
after its generation.
