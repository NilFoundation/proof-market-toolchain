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
    url = URL + f'_db/{DB_NAME}/{MOUNT}/statement/'
    res = requests.get(url=url, headers=getHeaders())
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        return res.json()


def getMyAsks(status='processing'):
    url = URL + f'_db/{DB_NAME}/{MOUNT}/ask/'
    url += f'?q=[{{"key" : "sender", "value" : "{USER}"}},{{"key" : "status", "value" : "{status}"}}]'

    res = requests.get(url=url, headers=getHeaders())
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        return res.json()


def generateAsks():
    while True:
        # time.sleep(ASK_UPDATE_INTERVAL)
        createdAsks = getMyAsks('created')
        statements = MY_STATEMENTS

        for st in statements:
            isFound = False
            for ask in createdAsks:
                if ask['statement_key'] == st:
                    isFound = True
                    next
            if isFound == False:
                ask = {
                    "statement_key": st,
                    "cost": random.randint(0, 9)
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


def produceProof(ask):
    try:
        proof = open(PROOFS_DIR+ask['statement_key']+".txt", "r").read()
    except:
        logging.error(f"Error: proof file not found")
        return
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


def produceProofs():
    while True:
        matchedAsks = getMyAsks('processing')
        for ask in matchedAsks:
            time.sleep(WAIT_BEFORE_SEND_PROOF)
            produceProof(ask)


if __name__ == "__main__":
    update_auth()
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    Thread(target=produceProofs).start()
    Thread(target=generateAsks).start()
