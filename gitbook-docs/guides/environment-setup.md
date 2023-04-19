---
description: Environment setup for proof market cli
---

# Environment Setup

In this guide, we set up the packages/libraries we require in order to interact with the proof market using command-line utilities.

## Dependencies

On \*nix systems, the following dependencies need to be present & can be installed using the following command.

```shell
sudo apt install build-essential libssl-dev cmake clang-12 git autoconf libc-ares-dev libfmt-dev gnutls-dev liblz4-dev libprotobuf-dev libyaml-cpp-dev libhwloc-dev pkg-config xfslibs-dev systemtap-sdt-dev
```

### Boost

Users need to install boost either manually or from their distros repository. Please ensure you are installing version 1.76. Follow the guide to install [version 1.76](https://www.boost.org/doc/libs/1\_76\_0/more/getting\_started/unix-variants.html) manually

{% hint style="danger" %}
We are aware of compilation issues with boost and clang. Please ensure you have&#x20;

following versions:

* Boost == 1.76&#x20;
* clang == 12
{% endhint %}

We have tested the following set of versions of the libraries.

```
clang-12
clang++12
boost == 1.76
cmake >= 3.22.1
autoconf >= 2.71
automake >=  1.16.5
libc-ares-dev >= 1.18.1
libfmt-dev >= 8.1.1
liblz4-dev >= 1.9.3
gnutls-dev >= 7.81
libprotobuf-dev >= 3.12.4
libyaml-cpp-dev >= 0.2.2
libhwloc-dev >= 2.7.0
libsctp-dev >= 1.0.19
ragel >= 6.10
```
