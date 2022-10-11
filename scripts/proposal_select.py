import random
import sys
import requests
import json

secret = open(".secret", "r").read()
user = open(".user", "r").read()

if __name__ == "__main__":
        order = sys.argv[1]
        proposal = sys.argv[2]
        data = {"order_id": order, 'proposal': proposal}

        url = 'http://try.dbms.nil.foundation/market/proposal/select'
        res = requests.post(url=url, json=data, auth=(user, secret))
        if res.status_code != 200:
                print(res.reason)