import requests
import os
import argparse
import json
import logging

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def push(args):
    f = open(args.settings, 'r')
    settings = json.load(f)
    data = {
                'work_capacity': settings["capacity"],
                'expected_cost': [{'circuit_id': c["circuit_id"], 'cost': c["cost"]} for c in settings["circuits"]],
                'expected_eval_time': [{'circuit_id': c["circuit_id"], 'eval_time': c["generation_time"]} for c in settings["circuits"]],
        }
    
    url = 'http://try.dbms.nil.foundation/market/worker'
    res = requests.post(url=url, json=data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"New worker:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--settings', required=True, 
                        help='<Required> Path to JSON file with worker settings')
    args = parser.parse_args()
    push(args=args)