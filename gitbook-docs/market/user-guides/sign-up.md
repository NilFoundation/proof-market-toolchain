# Sign up

All access to market requires authentication. Please ensure you have a valid username/password. If you have not registered , please look at instructions on how to [here](../front-end.md#new-user-signup) via front end .

Or , you can use the below command line in the `proof-market-toolchain` repository.



```
python3 signup.py -u <username> -p <password> -e <e-mail>
```



Create a _.user_ and a _.secret_ file and add your username and password to it,&#x20;

You should do this inside the _scripts_ directory in the proof market tool-chain repository.

`.user` file should consist of your username (without newline)

```
username
```

`.secret` file should consist of your password(without newline)

```
password
```
