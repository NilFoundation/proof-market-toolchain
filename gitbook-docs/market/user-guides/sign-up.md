# Sign up

Access to the market requires authentication. Please ensure you have a valid username/password. If you have not registered, please look at the instructions on how to do so [here](../front-end.md#new-user-signup) via the front end.

Or, you can use the below command line in the `proof-market-toolchain` repository.

## User

Users can submit/retrieve orders on the proof market. You cannot generate proofs if you sign up as an ordinary user.

```
python3 scripts/signup.py user -u <username> -p <password> -e <e-mail>
```

## Proof Producer

Proof Producers can submit/retrieve orders on the proof market and have the additional ability to submit/generate proofs.&#x20;

```
python3 scripts/signup.py producer -u <username> -p <password> -e <e-mail>
```

If successful, this creates a _.user_ and a _.secret_ file in `scripts/`  the directory with your credentials.

