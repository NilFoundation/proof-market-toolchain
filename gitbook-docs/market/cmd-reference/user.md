---
description: Scripts to manage user
---

# user

User scripts are used to manage user credentials/accounts

## New User

To see all parameters which can be passed to the script:

```
python3 signup.py -h
```

Output

```
usage: signup.py [-h] -u USER -p PASSWD -e EMAIL

options:
  -h, --help            show this help message and exit
  -u USER, --user USER  user name
  -p PASSWD, --passwd PASSWD
                        password
  -e EMAIL, --email EMAIL
                        email

```

Ex:

```
python3 signup.py -u helloworld -p password123 -e hello@test.com
```

Output:

### **Success**

```
{"user":"helloworld","active":true,"extra":{},"error":false,"code":201}
```

### **Error#1: Invalid username format**

```
Error: 400 {"code":409,"error":true,"errorMessage":"Inavlid format for username"}
```

The username does not support special characters and can only be created with a combination of alphabets and numbers.

**Error#2: Duplicate user**

```
Error: 500 {"error":true,"errorNum":1702,"errorMessage":"duplicate user","code":500}
```
