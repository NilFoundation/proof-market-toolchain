import random
import sys
import requests
import json

secret = open(".secret", "r").read()
user = open(".user", "r").read()

if __name__ == "__main__":
    base_path = sys.argv[1]
    f = open(base_path)
    json_data = json.load(f)

    data = {"circuit_id": 2, "public_input": "", "sender": hex(random.randint(10 ** 40, 10 ** 41)),
            "wait_period": 1000, 'public_input': json_data['public_input']}

    url = 'http://try.dbms.nil.foundation/market/order'
    res = requests.post(url=url, json=data, auth=(user, secret))
    print(res.status_code)
    if res.status_code != 200:
        print(res.reason)