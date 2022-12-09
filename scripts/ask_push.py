import requests
import json
import os
import logging
import json
import argparse
import numpy as np

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def push(args):
    data = {"circuit_id": args.circuit_id,
            "sender": args.sender,
            "wait_period": 1000, 
            "cost": args.cost, 
            "eval_time": args.generation_time,
            }
    url = 'http://try.dbms.nil.foundation/market/ask'
    res = requests.post(url=url, json=data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Limit ask:\t {json.dumps(res.json(), indent=4)}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--cost', metavar='cost', type=float,
                        help='cost', required=True)
    parser.add_argument('--sender', metavar='sender', type=str,
                        help='sender', required=True)
    parser.add_argument('--file', default=None, metavar='file', type=str,
                        help='load ask description from file')
    parser.add_argument('--circuit_id', metavar='circuit_id', type=int,
                        help='circuit_id', required=True)
    parser.add_argument('--generation_time', metavar='circuit_id', type=int,
                        help='proof time generation (in mins)', required=True)
    args = parser.parse_args()
    push(args=args)
