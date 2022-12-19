import requests
import sys
import logging
import argparse
import json
from constants import DB_NAME, URL, MOUNT

def get(args):
    headers = {}
    with open(args.auth, 'r') as f:
        auth = json.load(f)
        headers.update(auth)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/' 
    if args.key:
        url += args.key
    elif args.status:
        url += f'?q=[{{"key" : "status", "value" : "{args.status}"}}]'
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        if args.verbose:
            logging.info(f"Bids:\n {json.dumps(res.json(), indent=4)}")
        else:
            bids = res.json()
            for bid in bids:
                del bid['input']
            logging.info(f"Bids:\n {json.dumps(bids, indent=4)}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('-k', '--key', metavar='key', type=str,
                        help='key of the bid')
    parser.add_argument('--auth', metavar='auth', type=str, default='auth.json',
                        help='auth')
    parser.add_argument('-s', '--status', metavar='status', type=str,
                        help='status of bids')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='verbose')
    args = parser.parse_args()
    get(args)