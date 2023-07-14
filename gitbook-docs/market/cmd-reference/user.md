---
description: Creating and managing accounts
---

# User account utilities

This page describes the script managing user credentials/account — `signup.py`.

## Creating a new account
### Usage

To see all parameters that can be passed to the script, run it with the `-h` option:

```console
$ python3 signup.py -h

usage: signup.py [-h] -u USER -p PASSWD -e EMAIL

options:
  -h, --help            show this help message and exit
  -u USER, --user USER  user name
  -p PASSWD, --passwd PASSWD
                        password
  -e EMAIL, --email EMAIL
                        email
```

### Example

```bash
python3 signup.py -u helloworld -p password123 -e hello@test.com
```

### Possible results

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

## Signing in

If you already have an account, store your username and password in files `.user` and `.secret`
in the `./scripts` folder:

```console
$ echo '<username>' > .user
$ echo '<password>' > .secret
```
