import random
import sys
import requests
import logging
import argparse
import os.path
import re
import importlib  
import inspect
import math

currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0, parentdir) 

placeholder = importlib.import_module("libs.placeholder-research.src")


def proof_price(params):

    s = str(re.escape("rows_amount : "))
    e = str(re.escape(","))    

    parse_res = re.findall(s + "(.*)" + e, params)

    if(parse_res == []):
        rows = 8
    else:
        rows = int(parse_res[0])

    # TODO add normal parsed data in Proof
    wires = 20
    perm = 10
    const = 1
    gate_add_rotation = placeholder.redshift.RotationMap(wires)
    gate_add_rotation.add_shifts([0, 1], [2, -1, 2])
    gate_mul_rotation = placeholder.redshift.RotationMap(wires)
    gate_add = placeholder.redshift.Gate(gate_add_rotation, 1)
    gate_mul = placeholder.redshift.Gate(gate_mul_rotation, 2)
    gates = [gate_add, gate_mul]
    sel = 3
    lookup_rows = 2 ** 18
    lookup_columns = 2

    localization_factor = 2
    step = 3

    circuit = placeholder.redshift.CircuitParams(rows, wires, perm, const, sel, gates, lookup_rows, lookup_columns)
    fri = placeholder.redshift.FriParams(0.0000001, 40, 16, localization_factor, 2 ** (math.log2(rows) + 4), math.log2(rows), step)
    redshift = placeholder.redshift.RedshiftParams(fri, 256, 64)
    proof = placeholder.proof_versions.ProofV4(circuit, redshift)
    return placeholder.price_plot_drawer.price(proof)

def push(data=None, args=None):
    if data is None and args:
        bid_id = args.bid_id
        # try read proof from args.file, if not, get exception
        try:
            proof = open(args.file, "r").read()
        except:
            logging.error(f"Error: proof file not found")
            return
        data = {"bid_id": bid_id, "proof": proof}
    
    url = 'http://try.dbms.nil.foundation/market/proof_circuit_type'
    res = requests.post(url=url, json=data, auth=(user, secret))

    if res.status_code != 200:
        logging.error(f"Did not find the type of circuit. Error: {res.status_code} {res.reason}")
        return
    
    price = proof_price(res.json())
    data["approx_price"] = "{:.2e}".format(price)

    url = 'http://try.dbms.nil.foundation/market/proof'
    res = requests.post(url=url, json=data, auth=(user, secret))
    if res.status_code != 200:
        logging.error(f"Error: {res.status_code} {res.json()}")
        return
    else:
        logging.info(f"Pushed proof:\t {res.json()}")
        return res.json()

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(message)s')
    parser = argparse.ArgumentParser()
    # parse bid_id and file
    parser.add_argument('-b', '--bid_id', metavar='bid_id', type=int, help='bid_id', required=True)
    parser.add_argument('-p', '--proof', metavar='path to proof file', type=str, help='file', required=True)
    args = parser.parse_args()
    push(args=args)
        