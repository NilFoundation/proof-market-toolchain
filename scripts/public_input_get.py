import requests
import sys
import logging
import argparse
import json
from constants import DB_NAME, URL, MOUNT
from auth_tools import get_headers

def get(args):
    headers = get_headers(args)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/' 
    if args.key:
        url += args.key
    res = requests.get(url=url, headers=headers)
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        return res

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('-k', '--key', metavar='key', type=str,
                        help='key of the bid')
    parser.add_argument('--auth', metavar='auth', type=str,
                        help='auth')
    parser.add_argument('-o', '--output', metavar='file', type=str, required=True,
                        help='output file')
    args = parser.parse_args()
    res = get(args)
    with open(args.output, 'w') as f:
        output = res.json()['input']
        json.dump(output, f, indent=4)