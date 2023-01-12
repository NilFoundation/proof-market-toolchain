import logging
import json
import argparse
import requests
from constants import DB_NAME, URL, MOUNT

def push(data=None, args=None):
    if data is None and args:
        f = open(args.file, 'r')
        input = json.load(f)
        data = {
                "statement_key": args.key,
                'input': input,
                "cost": args.cost, 
                }

    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code!=200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Limit bid:\t {res.json()}")
        return res.json()

def get(args):
    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/' 
    if args.bid_status:
        url += f'?q=[{{"key" : "status", "value" : "{args.bid_status}"}}]'
    elif args.key:
        url += args.key
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Bids:\n {json.dumps(res.json(), indent=4)}")
        return res.json()

def get_headers(args):
    headers = {}
    with open(args.auth, 'r') as f:
        auth = json.load(f)
        headers.update(auth)
    return headers

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--auth', type=str, default='auth.json',
                        help='auth file')
    subparsers = parser.add_subparsers(help='sub-command help')
    parser_push = subparsers.add_parser('push', help='push bid')
    parser_push.set_defaults(func=push)
    parser_get = subparsers.add_parser('get', help='get bid')
    parser_get.set_defaults(func=get)
    parser_get.add_argument('--key', type=str,
                        help='bid key')
    parser_get.add_argument('--bid_status', type=str,
                        help='bid status')
    parser_push.add_argument('--cost', type=float, required=True,
                        help='cost')
    parser_push.add_argument('--file', type=str, required=True,
                        help='file with public input')
    parser_push.add_argument('--key', type=str, required=True,
                        help='statement key')
    parser_push.add_argument('--generation_time', default=30, type=int,
                        help='required proof time generation (in mins)')    
    args = parser.parse_args()
    args.func(args=args)
