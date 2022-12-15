import requests
import json
import logging
from constants import DB_NAME, URL, MOUNT
import argparse

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('--auth', metavar='auth', type=str, default='auth.json',
                        help='auth')
    parser.add_argument('-f', '--file', metavar='file', type=str, default=None,
                        help='file to write proof')
    parser.add_argument('-k', '--key', metavar='key', type=str,
                        help='key of the proof')
    parser.add_argument('-b', '--bid_key', metavar='bid_key', type=str,
                        help='bid_key')
    args = parser.parse_args()
    
    headers = {}
    with open(args.auth, 'r') as f:
        auth = json.load(f)
        headers.update(auth)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/proof/'
    if args.bid_key:
        url += f'?bid_key={args.bid_key}'
    elif args.key:
        url += args.key
    else:
        logging.error("Error: bid_key or key must be specified")
        exit(1)
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.reason}")
        exit(1)
    else:
        res_json = res.json()
        proof_data = res_json
        if args.file:
            with open(args.file, 'w') as f:
                f.write(proof_data)
        logging.info(f"Proof:\t\t {res_json}")
        

    logging.basicConfig(level=logging.INFO, format='%(message)s')