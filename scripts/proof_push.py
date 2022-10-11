import random
import sys
import requests
import json

secret = open(".secret", "r").read()
user = open(".user", "r").read()

if __name__ == "__main__":
        order_id = sys.argv[1]
        data = {"order_id": order_id, "proof": open(sys.argv[2]).read()}

        url = 'http://try.dbms.nil.foundation/market/proof'
        res = requests.post(url=url, json=data, auth=(user, secret))
        if res.status_code != 200:
                print(res.reason)