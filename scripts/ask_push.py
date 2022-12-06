import random
import sys
import requests
import json
import os
import logging
import json
import argparse
import numpy as np

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def push(data=None):
    if data is None:
        if len(sys.argv) >= 2:
            public_input = sys.argv[1]
        else:
            public_input = '8690a3b1428829d4ac104b93468bf8495e79699c424aab7b1cf94e5f78b3bcc8'

        data = {"circuit_id": 2, 
                "sender": 'James',
                "wait_period": 1000, 
                'public_input': public_input,
                "cost": random.randint(1, 100), 
                "eval_time": random.randint(5, 100),
                }

def push(data=None, args=None):
    if data is None and args:
        data = {"circuit_id": args.circuit_id,
                "sender": args.sender,
                "wait_period": 1000, 
                "cost": args.cost, 
                "eval_time": np.random.normal(1000, 300),
                }
    url = 'http://try.dbms.nil.foundation/market/ask'
    res = requests.post(url=url, json=data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Limit ask:\t {res.json()}")
        return json.loads(res.json())

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', default=42, metavar='seed', type=int, 
                        help='seed')
    parser.add_argument('--cost', default=0.5, metavar='cost', type=float,
                        help='cost')
    parser.add_argument('--sender', default=random.randint(0,12), metavar='sender', type=int,
                        help='sender')
    # parser.add_argument('--file', default=None, metavar='file', type=str,
    #                     help='file')
    parser.add_argument('--circuit_id', default=0, metavar='circuit_id', type=int,
                        help='circuit_id')
    args = parser.parse_args()
    random.seed(args.seed)
    push(args=args)
