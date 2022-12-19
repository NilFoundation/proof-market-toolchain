import requests
import json
import logging
import argparse
from constants import DB_NAME, URL, MOUNT

def push(args):
    if args.file:
        f = open(args.file, 'r')
        data = json.load(f)

    headers = {}
    if args.auth:
        with open(args.auth, 'r') as f:
            auth = json.load(f)
        headers.update(auth)

    url = URL + f'_db/{DB_NAME}/{MOUNT}/statement/'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code!=200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Statement:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--file', default=None, metavar='file', type=str,
                        help='file', required=True)
    parser.add_argument('--auth', default='auth.json', metavar='auth', type=str,
                        help='auth')
    args = parser.parse_args()
    push(args=args)
