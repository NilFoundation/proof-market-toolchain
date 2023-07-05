---
description: Environment setup for the Proof Market CLI
---

# Environment setup

This guide describes how to set up packages/libraries required to interact
with the Proof Market through the command line.

## Dependencies

You can install the required dependencies for Debian systems via the following command:

```bash
apt install \
    build-essential \
    libssl-dev \
    cmake \
    clang-12 \
    git \
    autoconf \
    libc-ares-dev \
    libfmt-dev \
    gnutls-dev \
    liblz4-dev \
    libprotobuf-dev \
    libyaml-cpp-dev \
    libhwloc-dev \
    pkg-config \
    xfslibs-dev \
    systemtap-sdt-dev
```

### Boost

[Boost](https://www.boost.org) can be installed either manually or from the distributive's repository.
Make sure you are installing **version 1.76**.
Follow [this guide](https://www.boost.org/doc/libs/1_76_0/more/getting_started/unix-variants.html)
to install this version manually.

{% hint style="info" %}
Use the recommended versions of the libraries to avoid compilation issues:

* Boost == 1.76
* clang == 12
{% endhint %}

We've tested the following versions of the libraries:

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
