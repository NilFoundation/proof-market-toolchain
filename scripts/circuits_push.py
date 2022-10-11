import requests
import json
import sys

secret = open(".secret", "r").read()
user = open(".user", "r").read()

# file_path = '/root/data/state.json'
if __name__ == "__main__":
    f = open(sys.argv[1])
    json_data = json.load(f)

    url = 'http://try.dbms.nil.foundation/market/circuit'
    res = requests.post(url=url, json=json_data, auth=(user, secret))
    print(res.status_code)
    if res.status_code != 200:
        print(res.reason)