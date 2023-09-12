import os

VERSION = open(
    os.path.dirname(os.path.abspath(__file__)) + "/../../VERSION", "r"
).read()
URL = "https://api.proof.market.nil.foundation/"
DB_NAME = "market"
MOUNT = "/v" + VERSION.replace(".", "_")
AUTH_FILE = "./.auth.json"
PROOFS_DIR = "./proofs/statements/"
WAIT_BEFORE_SEND_PROOF = 30
ASK_UPDATE_INTERVAL = 20
PROOF_GEN_CONFIG_FILE='./proof_generators.ini'
MY_STATEMENTS = {
    "32326": {"cost": 5, "proposals_limit": 10},
    "32292": {"cost": 5, "proposals_limit": 10},
    "79169223": {"cost": 5, "proposals_limit": 10},
}
USER = "skm"
REQUEST_TIMEOUT = 100
