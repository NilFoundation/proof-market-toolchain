import requests
import sys
import json
import os
import logging
import argparse

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

# file_path = '/root/data/state.json'
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', metavar='file', type=str, required=True,
                        help='file path')
    args = parser.parse_args()

    json_data = json.load(open(args.file))
    url = 'http://try.dbms.nil.foundation/market/circuit'
    res = requests.post(url=url, json=json_data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        sys.exit(1)
    else:
        logging.info(f"Circuit {json_data['name']}:\t {res.status_code}")
        sys.exit(0)