import random
import sys
import requests
import json

secret = open(".secret", "r").read()
user = open(".user", "r").read()

if __name__ == "__main__":
        order_id = sys.argv[1]
        proposal_id = sys.argv[2]
        data = {"bid": 123, "sender": hex(random.randint(10 ** 40, 10 ** 41)),
                "eval_time": random.randint(100, 10000), 'order': order_id}

        url = 'http://try.dbms.nil.foundation/market/proposal'
        res = requests.post(url=url, json=data, auth=(user, secret))
        if res.status_code != 200:
                print(res.reason)