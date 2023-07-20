# Proof requester

{% hint style="info" %}
[Sign up](sign-up.md) and keep the authentication files in order to use
the following command line tools.
{% endhint %}

## Submit a request

A request order has additional details, such as who requests the proof and what they are willing
to pay for it.
The proof requester can create an order like this:

```bash
python3 scripts/request_tools.py push \
    --cost <cost of the request> \
    --file <json file with public_input> \
    --key <key of the statement> 
```

## Check order status

Proof requesters can check their requests with the following command:

```bash
python3 scripts/request_tools.py get --key <key of the request>
```

## Obtain the proof

The proof requester can get their proof either by request key or proof key:

```bash
python3 scripts/proof_tools.py get --request_key <key of the request>s
```
