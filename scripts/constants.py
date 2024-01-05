import os

VERSION = open(os.path.dirname(os.path.abspath(__file__)) + "/../VERSION", "r").read()
URL = "http://49.12.15.40:3000"
DB_NAME = "market"
MOUNT = "/v" + VERSION.replace(".", "_")
REQUEST_TIMEOUT = 100
