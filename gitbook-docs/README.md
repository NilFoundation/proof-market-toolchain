---
description: Market for non-interactive zero-knowledge proofs
---

# `=nil;` Proof Market

Generating non-interactive zero-knowledge proofs (zkProofs) is a compute-heavy task that can take hours.
To generate proofs reliably and cost-effectively, one needs to develop and maintain
a computational infrastructure, scale it, and specialize as a proof producer.

`=nil;` understands that generating proofs is a task that should be outsourced to such
a specialized provider.
This is exactly why we are building Proof Market — a marketplace where everyone can request a zkProof,
and a network of specialized producers will respond to such requests.

We see Proof Market as the place where proof requesters and proof producers meet
and create a free, open, and self-sustaining market.

{% hint style="info" %}
Currently, Proof Market accepts circuits generated using our zkLLVM compiler
and based on the [Placeholder](https://github.com/NilFoundation/evm-placeholder-verification) proof system.
However, the ultimate goal is to enable validation of any proofs from any system,
for instance, AIR, R1CS-based, and other PLONK-based proofs.
If you wish to use Proof Market for your needs, please get in touch with the team
on our [contacts](misc/contact.md) page.
{% endhint %}

{% hint style="warning" %}
Proof Market is currently in its beta version.
We expect there will be continuous changes in it following the feedback we receive from the customers.
{% endhint %}
