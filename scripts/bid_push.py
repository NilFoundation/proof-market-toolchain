import requests
import os
import logging
import json
import argparse

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()


def push(data=None, args=None):
    if data is None and args:
        f = open(args.public_input, 'r')
        public_input = json.load(f)

        data = {"circuit_id": args.circuit_id,
                "sender": args.sender,
                "wait_period": 1000, 
                'public_input': public_input,
                "cost": args.cost, 
                "eval_time": args.generation_time,
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
        logging.info(f"Limit bid:\t {json.dumps(res.json(), indent=4)}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('--cost', metavar='cost', type=float,
                        help='cost', required=True)
    parser.add_argument('--sender', metavar='sender', type=str,
                        help='sender', required=True)
    parser.add_argument('--file', default=None, metavar='file', type=str,
                        help='load bid description from file')
    parser.add_argument('--public_input', metavar='public_input', type=str,
                        help='public_input', required=True)
    parser.add_argument('--circuit_id', metavar='circuit_id', type=int,
                        help='circuit_id', required=True)
    parser.add_argument('--generation_time', default=30, metavar='circuit_id', type=int,
                        help='required proof time generation (in mins)')
    args = parser.parse_args()
    push(args=args)
