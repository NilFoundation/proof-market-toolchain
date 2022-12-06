import requests
import sys
import os
import logging
import argparse
import json

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('--id', metavar='id', type=int, required=True,
                        help='bid id')
    args = parser.parse_args()
    url = 'http://try.dbms.nil.foundation/market/ask/id/' + str(args.id)
    res = requests.get(url=url, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        sys.exit(1)
    else:
        logging.info(f"Ask:\n {json.dumps(res.json(), indent=4)}")
        sys.exit(0)
