import argparse
from constants import URL, DB_NAME, MOUNT
import requests
from auth_tools import get_headers, create_credentials_file


def signup(user, password, email):
    url = URL + f"_db/{DB_NAME}/{MOUNT}/user/signup"
    body = {
        "user": user,
        "passwd": password,
        "email": email,
    }

    response = requests.post(url, json=body)
    if response.status_code != 200:
        print(f"Error: {response.status_code} {response.text}")
    else:
        print(response.text)
    return response


def register_producer(description, url, logo, eth_address):
    headers = get_headers(None)
    url = URL + f"_db/{DB_NAME}/{MOUNT}/producer/register"
    body = {"description": description}
    if url is not None:
        body["url"] = url
    if logo is not None:
        body["logo"] = logo
    if eth_address is not None:
        body["eth_address"] = eth_address
    response = requests.post(url, json=body, headers=headers)
    if response.status_code != 200:
        print(f"Error: {response.status_code} {response.text}")
    else:
        print(response.text)


def signup_parser(args):
    response = signup(args.user, args.password, args.email)
    if response.status_code == 200:
        create_credentials_file("secret", args.password)
        create_credentials_file("user", args.user)


def register_producer_parser(args):
    register_producer(args.description, args.url, args.logo, args.eth_address)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(help="sub-command help")
    parser_user = subparsers.add_parser("user", help="user signup")
    parser_user.set_defaults(func=signup_parser)
    parser_producer = subparsers.add_parser("producer", help="producer signup")
    parser_producer.set_defaults(func=register_producer_parser)

    parser_user.add_argument(
        "-u", "--user", action="store", required=True, help="user name"
    )
    parser_user.add_argument(
        "-p", "--password", action="store", required=True, help="password"
    )
    parser_user.add_argument(
        "-e", "--email", action="store", required=True, help="email"
    )
    parser_producer.add_argument(
        "-d",
        "--description",
        action="store",
        default="Generic Producer",
        help="description of a producer",
    )
    parser_producer.add_argument(
        "-u", "--url", action="store", default=None, help="url of a producer"
    )
    parser_producer.add_argument(
        "-l", "--logo", action="store", default=None, help="logo of a producer"
    )
    parser_producer.add_argument(
        "-e", "--eth_address", action="store", default=None, help="eth address"
    )
    args = parser.parse_args()

    args.func(args)
