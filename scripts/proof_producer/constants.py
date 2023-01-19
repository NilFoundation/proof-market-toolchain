import os

VERSION = open(os.path.dirname(os.path.abspath(__file__)) + "/../../VERSION", "r").read()
URL = 'https://api.proof.market.nil.foundation/'
DB_NAME = 'market'
MOUNT = '/v' + VERSION.replace('.', '_')
AUTH_FILE='./.auth.json'
PROOFS_DIR= './proofs/statements/'
WAIT_BEFORE_SEND_PROOF = 30
ASK_UPDATE_INTERVAL=20
MY_STATEMENTS={'31983' : 2, }
USER='root'