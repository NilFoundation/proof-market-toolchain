import logging
import json
import argparse
import requests
from constants import DB_NAME, URL, MOUNT

def push(data=None, args=None):
    if data is None and args:
        f = open(args.file, 'r')
        input = f.read()
        data = {
                "statement_key": args.key,
                'input': input,
                "cost": args.cost, 
                "eval_time": args.generation_time,
                }

    headers = {}
    if args.auth:
        with open(args.auth, 'r') as f:
            auth = json.load(f)
        headers.update(auth)

    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/'
    res = requests.post(url=url, json=data, headers=headers)
    if res.status_code!=200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Limit bid:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--cost', metavar='cost', type=float, required=True,
                        help='cost')
    parser.add_argument('-f', '--file', default=None, metavar='file', type=str,
                        help='file', required=True)
    parser.add_argument('-k', '--key', default=0, metavar='statement_key', type=str,
                        help='key of the statement')
    parser.add_argument('--auth', metavar='auth', type=str, default='auth.json',
                        help='auth file')
    parser.add_argument('--generation_time', default=30, metavar='circuit_id', type=int,
                        help='required proof time generation (in mins)')
    args = parser.parse_args()
    push(args=args)
