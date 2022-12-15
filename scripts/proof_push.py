import requests
import logging
import argparse
import json
from constants import DB_NAME, URL, MOUNT

def push(data=None, args=None):
    if data is None and args:
        try:
            proof = open(args.file, "r").read()
        except:
            logging.error(f"Error: proof file not found")
            return
        data = {
                "proof": proof,
                "ask_key" : args.ask_key,
                "bid_key" : args.bid_key,
                }

    headers = {}
    if args.auth:
        with open(args.auth, 'r') as f:
            auth = json.load(f)
        headers.update(auth)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/proof'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Pushed proof:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('-a', '--ask_key', metavar='ask_key', type=str, 
                        help='ask_key', required=True)
    parser.add_argument('-b', '--bid_key', metavar='bid_key', type=str, 
                        help='bid_key', required=True)
    parser.add_argument('-f', '--file', metavar='file', type=str, 
                        help='file', required=True)
    parser.add_argument('--auth', metavar='auth', type=str, default='auth.json',
                        help='auth')
    args = parser.parse_args()
    push(args=args)
        