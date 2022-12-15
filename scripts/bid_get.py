import requests
import sys
import logging
import argparse
import json
from constants import DB_NAME, URL, MOUNT

def get(args, status=None):
    headers = {}
    with open(args.auth, 'r') as f:
        auth = json.load(f)
        headers.update(auth)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/' 
    if args.key:
        url += args.key
    elif status:
        url += f'?status={status}'
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        logging.info(f"Bids:\n {json.dumps(res.json(), indent=4)}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('-k', '--key', metavar='key', type=str,
                        help='key of the bid')
    parser.add_argument('--auth', metavar='auth', type=str, default='auth.json',
                        help='auth')
    args = parser.parse_args()
    get(args)