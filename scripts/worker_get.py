import requests
import sys
import os
import logging
import argparse
import json

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def get(args=None):
    if args and args.id:
        url = 'http://try.dbms.nil.foundation/market/worker/id/' + str(args.id)
    else:
        url = 'http://try.dbms.nil.foundation/market/worker'
    res = requests.get(url=url, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        sys.exit(1)
    else:
        return res.json()
        
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('--id', metavar='id', type=int,
                        help='worker id')
    args = parser.parse_args()
    result = get(args)
    logging.info(f"Worker:\n {json.dumps(result, indent=4)}")