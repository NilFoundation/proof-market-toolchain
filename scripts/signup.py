import argparse
from constants import URL, DB_NAME, MOUNT
import requests

def signup(args):

    url = URL + f'_db/{DB_NAME}/{MOUNT}/user/signup'
    body = {
            "user": args.user,
            "passwd": args.passwd,
            "email": args.email,
            }

    response = requests.post(url, json=body)
    if response.status_code != 200:
        print(f"Error: {response.status_code} {response.text}")
    else:
        print(response.text)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-u', '--user', action='store', required=True,
                        help='user name')
    parser.add_argument('-p', '--passwd', action='store', required=True,
                        help='password')
    parser.add_argument('-e', '--email', action='store', required=True,
                        help='email')
    args = parser.parse_args()

    signup(args)
