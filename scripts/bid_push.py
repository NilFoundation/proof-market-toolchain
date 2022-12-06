import random
import sys
import requests
import os
import logging
import json
import argparse
import numpy as np

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()


def push(data=None, args=None):
    if data is None and args:
        if args.public_input:
            public_input = args.public_input
        else:
            public_input = [random.choice('0123456789abcdef') for _ in range(64)]
            public_input = ''.join(public_input)

        data = {"circuit_id": args.circuit_id,
                "sender": args.sender,
                "wait_period": 1000, 
                'public_input': public_input,
                "cost": args.cost, 
                "eval_time": np.random.normal(1000, 300),
                }
        if args.file:
            with open(args.file, 'r') as f:
                data = json.load(f)
            data['public_input'] = json.loads(data['public_input'])
    
    url = 'http://try.dbms.nil.foundation/market/bid'
    res = requests.post(url=url, json=data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Limit bid:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', default=42, metavar='seed', type=int, 
                        help='seed')
    parser.add_argument('--cost', default=0.5, metavar='cost', type=float,
                        help='cost')
    parser.add_argument('--sender', default='root', metavar='sender', type=str,
                        help='sender')
    parser.add_argument('--file', default=None, metavar='file', type=str,
                        help='file')
    parser.add_argument('--public_input', default=None, metavar='public_input', type=str,
                        help='public_input')
    parser.add_argument('--circuit_id', default=0, metavar='circuit_id', type=int,
                        help='circuit_id')
    args = parser.parse_args()
    # print(args)
    random.seed(args.seed)
    push(args=args)
