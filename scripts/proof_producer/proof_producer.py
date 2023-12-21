"""Proof Producer daemon PoC"""
import requests
import sys
import logging
import argparse
import json
import time
import os
import random
from threading import Thread
from constants import (
    MY_STATEMENTS,
    DB_NAME,
    URL,
    MOUNT,
    USER,
    AUTH_FILE,
    PROOFS_DIR,
    ASK_UPDATE_INTERVAL,
)
import subprocess

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../")
from public_input_get import get as get_public_input
from auth_tools import update_auth, get_headers
from statement_tools import get as get_statement
from proposal_tools import push as push_proposal
from proof_tools import push as push_proof

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read()
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read()


def auth_loop():
    update_auth(AUTH_FILE)
    wait_time = 20 * 60  # 20 minutes
    while True:
        time.sleep(wait_time)
        update_auth(AUTH_FILE)


def get_statements(auth):
    keys = MY_STATEMENTS.keys()
    statements = {}
    for key in keys:
        try:
            statements[key] = get_statement(auth, key, None)
        except:
            logging.error(f"Get statement error")
            continue
    return statements


def get_my_proposals(status="processing"):
    url = URL + "/proposal"
    url += f'?status={status}'
    res = requests.get(url=url, headers=get_headers(AUTH_FILE))
    if res.status_code != 200:
        logging.error(f"Get my proposals error: {res.status_code} {res.text}")
        sys.exit(1)
    else:
        proposals = res.json()
        my_statements_proposals = []
        for proposal in proposals:
            if proposal["statement_key"] in MY_STATEMENTS:
                my_statements_proposals.append(proposal)
        return my_statements_proposals


def proposals_loop():
    while True:
        time.sleep(ASK_UPDATE_INTERVAL)
        try:
            createdProposals = get_my_proposals("created")
            processingProposals = get_my_proposals("processing")
        except:
            logging.error(f"Get proposals error")
            continue

        for st in MY_STATEMENTS:
            proposalsFound = 0
            for proposal in createdProposals:
                if proposal["statement_key"] == st:
                    proposalsFound += 1
            for proposal in processingProposals:
                if proposal["statement_key"] == st:
                    proposalsFound += 1
            if proposalsFound < MY_STATEMENTS[st]["proposals_limit"]:
                cost = MY_STATEMENTS[st]["cost"] + round(random.uniform(0, 1), 1)
                push_proposal(AUTH_FILE, st, cost)


def produce_proof(proposal, proof_generator_binary_path, assigner_binary_path, log_level, auth):
    circuit = "./statements/" + proposal["statement_key"] + ".json"
    try:
        public_input_data = get_public_input(proposal["request_key"], auth).json()["input"]
    except:
        logging.error(f"Get public input error for proposal: {proposal['_key']}")
        return
    pulic_input_file = "public_input.json"
    with open(pulic_input_file, "w") as f:
        json.dump(public_input_data, f, indent=4)

    try:
        with open(circuit, 'r') as file:
            data = json.load(file)
        bytecode_data = data['definition']['proving_key'] 
    except:
        logging.error(f"Get bytecode error for proposal: {proposal['_key']}")
        return
    bytecode_file = "bytecode.ll"
    with open(bytecode_file, "w") as f:
        json.dump(bytecode_data, f, indent=4)

    assignment_table_path = "assignment.tbl"
    circuit_path = "circuit.crct"
    assigner = subprocess.Popen(
        [
            assigner_binary_path,
            "--bytecode=" + bytecode_file,
            "--public-input=" + pulic_input_file,
            "--circuit=" + circuit_path,
            "--assignment-table=" + assignment_table_path,
            "--elliptic-curve-type=" + "pallas",
            "--log-level=" + log_level,
        ]
    )
    assigner.communicate()

    proof_path = "proof.bin"
    proof_generator = subprocess.Popen(
        [
            proof_generator_binary_path,
            "--circuit=" + circuit_path,
            "--assignment-table=" + assignment_table_path,
            "--proof=" + proof_path,
            "--log-level=" + log_level,
        ]
    )
    proof_generator.communicate()
    try:
        push_proof(auth, proof_path, request_key=proposal["request_key"], proposal_key=proposal["_key"])
    except:
        logging.error(f"Push proof error")
    return


def proofs_loop(proof_generator_binary_path, assigner_binary_path, log_level):
    while True:
        time.sleep(ASK_UPDATE_INTERVAL)
        try:
            matchedProposals = get_my_proposals("processing")
        except:
            logging.error(f"Get processing proposals error")
            continue
        for proposal in matchedProposals:
            produce_proof(proposal, proof_generator_binary_path, assigner_binary_path, log_level, AUTH_FILE)


def start(args):
    Thread(target=auth_loop).start()
    time.sleep(10)
    Thread(target=proposals_loop).start()
    Thread(target=proofs_loop(args.proof_generator, args.assigner, args.log_level)).start()


def prepare(args):
    update_auth(AUTH_FILE)
    statements = get_statements(AUTH_FILE)
    for key in statements:
        with open(args.statements + key + ".json", "w") as f:
            json.dump(statements[key], f, indent=4)
    logging.info(f"Statements prepared")


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")

    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_start = subparsers.add_parser(
        "start",
        help="start Proof Producer daemon  (do not forget to prepare statements first)",
    )
    parser_start.add_argument(
        "-s", "--statements", help="directory with statements", default="./statements/"
    )
    parser_start.add_argument(
        "--proof-generator", help="path to proof generator binary", required=True
    )
    parser_start.add_argument(
        "--assigner", help="path to assigner binary", required=True
    )
    parser_start.add_argument(
        "--log-level", help="log level", choices=['trace', 'debug', 'info', 'warning', 'error', 'fatal'], default="info"
    )
    parser_start.set_defaults(func=start)
    parser_prepare = subparsers.add_parser(
        "prepare",
        help="download statements from Proof Market (do not forget to setup constants.py first)",
    )
    parser_prepare.add_argument(
        "-s", "--statements", help="directory with statements", default="./statements/"
    )

    parser_prepare.set_defaults(func=prepare)
    args = parser.parse_args()
    args.func(args)
