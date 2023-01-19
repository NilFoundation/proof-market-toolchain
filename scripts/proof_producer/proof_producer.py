import requests
import sys
import logging
import argparse
import json
import time
import os
import random
from threading import Thread
from constants import MY_STATEMENTS, DB_NAME, URL, MOUNT, USER, AUTH_FILE, PROOFS_DIR, WAIT_BEFORE_SEND_PROOF, ASK_UPDATE_INTERVAL
import subprocess

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()

def update_auth():

    url = URL + "_open/auth"
    body = {
            "username": user, 
            "password": secret
            }

    response = requests.post(url, json=body)
    if response.status_code != 200:
         logging.error(f"Error: {response.status_code} {response.json()}")
    else:
        with open(AUTH_FILE, "w") as f:
            headers = {
                'Authorization': f'Bearer {response.json()["jwt"]}'
            }
            json.dump(headers, f)

def getHeaders():
    headers = {}
    if AUTH_FILE:
        with open(AUTH_FILE, 'r') as f:
            auth = json.load(f)
        headers.update(auth)
    return headers


def getStatements():
    keys = MY_STATEMENTS.keys()
    statements = {}
    for key in keys:
        url = URL + f'_db/{DB_NAME}/{MOUNT}/statement/' 
        url += key
        res = requests.get(url=url, headers=getHeaders())
        if res.status_code != 200:
            logging.error(f"Error: {res.status_code} {res.text}")
        else:
            statements[key] = res.json()
    return statements


def getMyAsks(status='processing'):
    url = URL + f'_db/{DB_NAME}/{MOUNT}/ask/'
    url += f'?q=[{{"key" : "sender", "value" : "{USER}"}},{{"key" : "status", "value" : "{status}"}}]'

    res = requests.get(url=url, headers=getHeaders())
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        return res.json()

def getPublicInput(key):
    url = URL + f'_db/{DB_NAME}/{MOUNT}/bid/' 
    url = url + key
    res = requests.get(url=url, headers=getHeaders())
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        return res.json()['input']


def generateAsks():
    print("asks")
    while True:
        # time.sleep(ASK_UPDATE_INTERVAL)
        createdAsks = getMyAsks('created')
        processingAsks = getMyAsks('processing')
        statements = MY_STATEMENTS.keys()

        for st in statements:
            isFound = False
            for ask in createdAsks:
                if ask['statement_key'] == st:
                    isFound = True
                    next
            for ask in processingAsks:
                if ask['statement_key'] == st:
                    isFound = True
                    next
            if isFound == False:
                ask = {
                    "statement_key": st,
                    "cost": MY_STATEMENTS[st],
                }
                pushAsk(ask)


def pushAsk(ask):
    logging.info(f"pushAsk")
    url = URL + f'_db/{DB_NAME}/{MOUNT}/ask/'
    res = requests.post(url=url, json=ask, headers=getHeaders())
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        return
    else:
        logging.info(f"Ask pusshed:\t {res.json()}")
        return res.json()


def produceProof(ask, binary):
    circuit = "./statements/" + ask['statement_key'] + ".json"
    input = "/mnt/d/gits/proof-market-toolchain/example/input/arithmetic_example/input.json"
    input = getPublicInput(ask['bid_key'])
    input_file = "input.json"
    with open(input_file, "r") as f:
        json.dump(res.json(), f, indent=4)
    output = "proof"
    generator = subprocess.Popen([binary, "--circuit_input=" + circuit, "--public_input=" + input_file, "--proof_out="+output])
    generator.communicate()
    proof = open(output, "r").read()
    data = {
        "proof": proof,
        "ask_key": ask['_key'],
        "bid_key": ask['bid_key'],
    }

    url = URL + f'_db/{DB_NAME}/{MOUNT}/proof'
    res = requests.post(url=url, json=data, headers=getHeaders())
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Pushed proof for :\t {ask}")
        return res.json()


def produceProofs(binary_path):
    print("produce")
    while True:
        matchedAsks = getMyAsks('processing')
        for ask in matchedAsks:
            produceProof(ask, binary_path)

def start(args):
    update_auth()

    Thread(target=generateAsks).start()
    Thread(target=produceProofs(args.proof_generator)).start()

def prepare(args):
    update_auth()
    statements = getStatements()
    for key in statements:
        with open(args.directory+key+".json", "w") as f:
            json.dump(statements[key], f, indent=4)
    logging.info(f"Statements prepared")

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help='sub-command help')
    parser_start = subparsers.add_parser('start', help='start Proof Producer daemon  (do not forget to prepare statements first)')
    parser_start.add_argument("-s", "--statements", help="directory with statements", default="./statements/")
    parser_start.add_argument("-p", "--proof-generator", help="path to proof generator binaty", required=True)
    parser_start.set_defaults(func=start)
    parser_prepare = subparsers.add_parser('prepare', help='download statements from Proof Market (do not forget to setup constants.py first)')
    parser_prepare.add_argument("-d", "--directory", help="directory with statements", default="./statements/")
    
    parser_prepare.set_defaults(func=prepare)
    args = parser.parse_args()
    args.func(args)