---
description: Creating and managing accounts
---

# User account utilities

`=nil;` prepared a [toolchain](https://github.com/NilFoundation/proof-market-toolchain)
for interacting with the Proof Market.
This page describes the script managing user credentials/account — `signup.py`.

## Usage

To see all parameters that can be passed to the script, run it with the `-h` option:

```console
foo@bar:~$ python3 signup.py -h

usage: signup.py [-h] -u USER -p PASSWD -e EMAIL

options:
  -h, --help            show this help message and exit
  -u USER, --user USER  user name
  -p PASSWD, --passwd PASSWD
                        password
  -e EMAIL, --email EMAIL
                        email
```

## Example

```bash
python3 signup.py -u helloworld -p password123 -e hello@test.com
```

## Possible results

Success:

```json
{"user":"helloworld","active":true,"extra":{},"error":false,"code":201}
```

Error #1 — invalid username format:

```bash
Error: 400 {"code":409,"error":true,"errorMessage":"Inavlid format for username"}
```

The username does not support special characters and can only be a combination of letters
and numbers.

Error #2 — duplicate user:

```bash
Error: 500 {"error":true,"errorNum":1702,"errorMessage":"duplicate user","code":500}
```
## register as a proof producer

If you wish to submit generated proofs to the Proof Market,
you need to also register as a proof producer:

```console
foo@bar:~$ python3 signup.py producer --url https://example.com
```

This will return producer metadata in JSON format:

```json
{
  "_key": "...",
  "_id": "producer/...",
  "_rev": "_fneT7J2---",
  "description": "Generic Producer",
  "url": "https://example.com",
  "name": "username",
  "createdOn": 1677588821180,
  "updatedOn": 1677588821180
}
```
