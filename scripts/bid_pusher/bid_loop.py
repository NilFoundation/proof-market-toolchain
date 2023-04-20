import sys
import os
import time
import random
import json
import logging
import numpy as np

sys.path.insert(1, os.path.join(sys.path[0], '..'))

import bid_tools

AUTH = None
ERROR_WAIT_TIME = 3 * 60  # seconds

STATEMENTS = [
    {
        "statement_key": "79169223",
        "name": "account_mina",
        "bid_cost": 30,
        "message_wait_time": 2 * 60,  # seconds
        "input_file": "inputs/account_mina.json",
        "log_file": "bid_log_account_mina.log"
    },
    {
        "statement_key": "32292",
        "name": "mina_state",
        "bid_cost": 60,
        "message_wait_time": 10 * 60,  # seconds
        "input_file": "inputs/mina_state.json",
        "log_file": "bid_log_mina_state.log"
    }
]


def create_logger(log_file):
    logger = logging.getLogger(log_file)
    logger.setLevel(logging.INFO)
    
    file_handler = logging.FileHandler(log_file)
    file_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
    logger.addHandler(file_handler)
    
    return logger


def submit_order_for_statement(statement):
    input_file = os.path.dirname(os.path.abspath(__file__)) + f"/{statement['input_file']}"
    
    if statement['name'] == "account_mina":
        with open(input_file, 'r') as f:
            data = json.load(f)

        input_lines = data["input"].split('\n')
        input_lines[1] = str(random.randint(1, 2**256))
        data["input"] = '\n'.join(input_lines)

        with open(input_file, 'w') as f:
            json.dump(data, f)
    elif statement['name'] == "mina_state":
        with open(input_file, 'r') as f:
            data = json.load(f)

        new_hex = '0x' + ''.join([random.choice('0123456789ABCDEF') for _ in range(64)])
        data["input"]["data"]["blockchainVerificationKey"]["commitments"]["sigma_comm"][0][0] = new_hex

        with open(input_file, 'w') as f:
            json.dump(data, f)
    
    cost = np.random.normal(statement['bid_cost'], 1.5)
    result = bid_tools.push(AUTH, statement['statement_key'], input_file, cost, verbose=True)
    statement['logger'].info(f"Successfully pushed bid: {result['_key']}")


def main():
    for statement in STATEMENTS:
        log_file = os.path.dirname(os.path.abspath(__file__)) + f"/{statement['log_file']}"
        statement['logger'] = create_logger(log_file)
    
    while True:
        T = 60  # seconds
        for statement in STATEMENTS:
            p = T / statement['message_wait_time']
            if random.random() < p:
                try:
                    submit_order_for_statement(statement)
                except Exception as e:
                    statement['logger'].error(f"Fatal error in main loop: {e}")
                    time.sleep(ERROR_WAIT_TIME)
        
        time.sleep(T)  # Wait T seconds


if __name__ == "__main__":
    main()