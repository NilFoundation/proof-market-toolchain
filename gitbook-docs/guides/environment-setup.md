---
description: Environment setup for proof market cli
---

# Environment Setup

In this guide we setup packages/libraries we require in order to interact with the proof market using command line utilities.

## Dependencies

* [Boost](https://www.boost.org/) == 1.76.0&#x20;
* [cmake](https://cmake.org/) >= 3.5
* [clang](https://clang.llvm.org/) >= 14.0.6

On \*nix systems, the following dependencies need to be present & can be installed using the following command.

```shell
 sudo apt install build-essential libssl-dev libboost-all-dev cmake clang git python3.8
```

{% hint style="danger" %}
We are aware of a compilation issue with boost libs having version higher than 1.76. Please use version 1.76.
{% endhint %}

