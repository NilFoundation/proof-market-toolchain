import os
import json
import argparse


def prepare(circuit_file, output_file, name, statement_type, private):
    data = {
        "name": name,
        "description": "description",
        "url": "url",
        "input_description": "input_description",
        "type": statement_type,
        "isPrivate": private,
        "definition": {
            "verification_key": "verification_key",
            "proving_key": "proving_key",
        },
    }

    if not os.path.exists(circuit_file):
        raise FileNotFoundError(f"The circuit file '{circuit_file}' does not exist.")

    with open(circuit_file, "r") as f:
        circuit = f.read()
        data["definition"]["proving_key"] = circuit

    with open(output_file, "w") as f:
        json.dump(data, f, indent=4)

def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-c", "--circuit", type=str, required=True, help="Path to a zkLLVM circuit (*.ll)"
    )
    parser.add_argument(
        "-o",
        "--output",
        metavar="output_file_path",
        type=str,
        required=True,
        help="Output file",
    )
    parser.add_argument("-n", "--name", type=str, required=True,
                        help="Statement name that will be shown on the Proof Market")
    parser.add_argument(
        "-t",
        "--type",
        metavar="statement_type",
        type=str,
        required=True,
        help="Statement type: placeholder-zkllvm or placeholder-vanilla",
    )
    private = parser.add_mutually_exclusive_group(required=True)
    private.add_argument('--private', action='store_true',
                         help="Make a statement private: only accessible by ID")
    private.add_argument('--public', action='store_false',
                         help="Make a statement public: visible on the Proof Market")
    args = parser.parse_args()
    prepare(args.circuit, args.output, args.name, args.type, args.private)

if __name__ == "__main__":
    main()
