import argparse
from constants import URL, DB_NAME, MOUNT
import requests
from auth_tools import get_headers

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

def register_producer(args):
    headers = get_headers(None)
    url = URL + f'_db/{DB_NAME}/{MOUNT}/producer/register'
    body = {
        'description': 'test producer',
    }

    response = requests.post(url, json=body, headers=headers)
    if response.status_code != 200:
        print(f"Error: {response.status_code} {response.text}")
    else:
        print(response.text)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_user = subparsers.add_parser("user", help="user signup")
    parser_user.set_defaults(func=signup)
    parser_producer = subparsers.add_parser("producer", help="producer signup")
    parser_producer.set_defaults(func=register_producer)
    parser_user.add_argument('-u', '--user', action='store', required=True,
                        help='user name')
    parser_user.add_argument('-p', '--passwd', action='store', required=True,
                        help='password')
    parser_user.add_argument('-e', '--email', action='store', required=True,
                        help='email')
    args = parser.parse_args()

    args.func(args)
