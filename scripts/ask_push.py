import requests
import json
import logging
import json
import argparse
from constants import DB_NAME, URL, MOUNT

def push(data=None, args=None):
    if data is None and args:
        data = {
                "statement_key": args.key,
                "cost": args.cost,
                }
    headers = {}
    if args.auth:
        with open(args.auth, 'r') as f:
            auth = json.load(f)
        headers.update(auth)

    url = URL + f'_db/{DB_NAME}/{MOUNT}/ask/'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Limit ask:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--cost', metavar='cost', type=float, required=True,
                        help='cost')
    parser.add_argument('-k', '--key', metavar='statement_key', type=str, required=True,
                        help='key of the statement')
    parser.add_argument('--auth', metavar='auth', type=str, default='auth.json',
                        help='auth file')
    args = parser.parse_args()

    push(args=args)
