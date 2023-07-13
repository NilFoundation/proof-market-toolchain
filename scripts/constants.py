import os

VERSION = open(os.path.dirname(os.path.abspath(__file__)) + "/../VERSION", "r").read()
URL = "https://api.proof.market.nil.foundation/"
DB_NAME = "market"
MOUNT = "v" + VERSION.replace(".", "_")
AUTH_FILE = "./.auth.json"
REQUEST_TIMEOUT = 100
