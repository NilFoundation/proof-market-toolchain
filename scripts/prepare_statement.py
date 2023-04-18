import os
import json
import argparse

def prepare(circuit_file, output_file, name, statement_type):
    data = {
        "name": name,
        "description": "description",
        "url": "url",
        "input_description": "input_description",
        "type": statement_type,
        "isPrivate": False,
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
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-c", "--circuit", type=str, required=True, help="zkllvm compiler output"
    )
    parser.add_argument(
        "-o",
        "--output",
        metavar="output file",
        type=str,
        required=True,
        help="output file",
    )
    parser.add_argument("-n", "--name", type=str, required=True, help="name")
    parser.add_argument(
        "-t",
        "--type",
        type=str,
        required=True,
        help="statement type: placeholder-zkllvm or placeholder-vanilla",
    )
    args = parser.parse_args()
    prepare(args.circuit, args.output, args.name, args.type)
