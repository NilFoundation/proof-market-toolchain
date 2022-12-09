import requests
import sys
import os
import logging
import argparse

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def get(proof_id):
    url = 'http://try.dbms.nil.foundation/market/proof/id/' + proof_id
    res = requests.get(url=url, auth=(user, secret))
    if res.status_code == 200:
            return res.json()
    else:
        logging.error(f"Error:\t {res.status_code} {res.reason}")
        return

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    parser.add_argument('--id', help="Proof's id", required=True)
    parser.add_argument('--output', help="Output file path", required=True)

    args = parser.parse_args()
    proof_json = get(args.id)
    proof = proof_json["proof"]
    with open(args.output, "w") as f:
        f.write(proof)
    logging.info(f"Proof is stored to {args.output}")
        
