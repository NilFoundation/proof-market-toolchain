---
description: Market for non-interactive zero-knowledge proofs
---

# `=nil;` Proof Market

Generating non-interactive zero-knowledge proofs (zkProofs) is a compute-heavy task that can take hours.
To generate proofs reliably and cost-effectively, one needs to develop and maintain
a computational infrastructure, scale it, and specialize as a proof producer.

`=nil;` understands that generating proofs is a task that should be outsourced to such
a specialized provider.
This is exactly why we are building the Proof Market — a marketplace where everyone can request a zkProof,
and a network of specialized producers will respond to such requests.

We see the Proof Market as the place where proof requesters and proof producers will meet
and create a free, open, and self-sustaining market.



{% hint style="info" %}
Currently, the Proof Market accepts circuits generated using our zkLLVM compiler.
However, the ultimate goal is to enable validation of any proofs from any system.
{% endhint %}

{% hint style="warning" %}
The Proof Market is currently in its beta version.
We expect there will be continuous changes in it following the feedback we receive from the customers.
{% endhint %}
