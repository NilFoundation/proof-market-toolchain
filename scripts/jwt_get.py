import os
import argparse
import json
from constants import URL
import requests

secret = open(os.path.dirname(os.path.abspath(__file__)) + "/.secret", "r").read().strip('\n')
user = open(os.path.dirname(os.path.abspath(__file__)) + "/.user", "r").read().strip('\n')

def update_auth(args):

    url = URL + "_open/auth"
    body = {
            "username": user, 
            "password": secret
            }

    response = requests.post(url, json=body)
    if response.status_code != 200:
        print(f"Error: {response.status_code} {response.json()}")
    else:
        print(response.text)
        with open(args.auth, "w") as f:
            headers = {
                'Authorization': f'Bearer {response.json()["jwt"]}'
            }
            json.dump(headers, f)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-a', '--auth', action='store', default='auth.json',
                        help='file to store jwt token')
    args = parser.parse_args()

    update_auth(args)
