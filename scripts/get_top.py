import requests
import sys
import os
import json
import logging

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def get():
    url = 'http://try.dbms.nil.foundation/market/top'
    try:
        res = requests.get(url=url, auth=(user, secret))
    except requests.exceptions.ConnectionError:
        logging.error("Connection error")
        return []
    if res.status_code == 200:
        return res.json()
    return res.json()

def get_costs():
    top_list = get()
    costs = {}
    for top in top_list:
        if top['bid'] and top['ask']:
            top_ret = {'ask': top['ask']['cost'], 
                       'bid': top['bid']['cost']}
            logging.info(f"Top for circuit {top['circuit_id']}:\t {top_ret}")
            costs[top['circuit_id']] = top_ret
    return costs


if __name__ == "__main__":
    top_list = get_costs()
    print(top_list)
