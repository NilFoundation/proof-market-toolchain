import argparse
import requests
import sys
from urllib.parse import urljoin

from auth_tools import get_headers, create_credentials_file
from constants import URL


def signup(user, password, email, url):
    url = urljoin(url, "/user/signup")
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
    url = urljoin(url, "/producer")
    body = {"description": description}
    if url is not None:
        body["url"] = url
    if logo is not None:
        body["logo"] = logo
    if eth_address is not None:
        body["eth_address"] = eth_address

    response = requests.post(url, json=body, headers=headers)
    if response.status_code == 200:
        pass
    else:
        print(f"Error: {response.status_code} {response.text}")

    print(response.text)
    return response.status_code in [200]


def signup_parser(args) -> bool:
    response = signup(args.user, args.password, args.email, args.url)
    if response.status_code == 200:
        create_credentials_file("secret", args.password)
        create_credentials_file("user", args.user)
    else:
        print(f"Error: {response.status_code} {response.text}")

    return response.status_code in [200]


def register_producer_parser(args) -> bool:
    return register_producer(
        args.description, args.url, args.logo, args.eth_address,
    )


def main():
    parser = argparse.ArgumentParser()
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument(
        "--url", action="store", default=URL, help="url of a producer"
    )

    subparsers = parser.add_subparsers(help="sub-command help")

    parser_user = subparsers.add_parser(
        "user", help="user signup", parents=[parent_parser]
    )
    parser_user.set_defaults(func=signup_parser)

    parser_producer = subparsers.add_parser("producer", help="producer signup", parents=[parent_parser])
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
        "-l", "--logo", action="store", default=None, help="logo of a producer"
    )
    parser_producer.add_argument(
        "-e", "--eth_address", action="store", default=None, help="eth address"
    )
    parser_producer.set_defaults(func=register_producer_parser)

    args = parser.parse_args()
    if hasattr(args, 'func'):
        # valid submodules
        sys.exit(0 if args.func(args) else 1)

    parser.print_help()
    sys.exit(1)


if __name__ == "__main__":
    main()
