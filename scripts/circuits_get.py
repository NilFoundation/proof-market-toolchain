import requests
import sys
import os
import json
import logging
import argparse

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('--id', metavar='id', type=int, help='circuit id')
    parser.add_argument('-s', '--statistics', action='store_true', default=False, 
                        help='get statistics')
    args = parser.parse_args()
    if args.id:
        url = 'http://try.dbms.nil.foundation/market/circuit/id/' + str(args.id)
    elif args.statistics:
        url = 'http://try.dbms.nil.foundation/market/circuit/statistics'
    else:
        url = 'http://try.dbms.nil.foundation/market/circuit'
    res = requests.get(url=url, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.reason}")
        sys.exit(1)
    else:
        logging.info(f"Circuit:\n {json.dumps(res.json(), indent=4)}")
        sys.exit(0)
