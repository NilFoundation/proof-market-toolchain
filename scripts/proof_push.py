import random
import sys
import requests
import logging
import argparse
import os.path
import re
import importlib  
import inspect
import math

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def push(args, data=None):
    if data is None and args:
        bid_id = args.bid_id
        # try read proof from args.file, if not, get exception
        try:
            proof = open(args.proof, "r").read()
        except:
            logging.error(f"Error: proof file not found")
            return
        data = {"bid_id": bid_id, "proof": proof}

    url = 'http://try.dbms.nil.foundation/market/proof'
    res = requests.post(url=url, json=data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Pushed proof:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('-b', '--bid_id', metavar='bid_id', type=int, help='bid_id', required=True)
    parser.add_argument('-p', '--proof', metavar='proof', type=str, help='path to proof file', required=True)
    args = parser.parse_args()
    push(args=args)
        