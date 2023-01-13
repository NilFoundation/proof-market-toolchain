import json
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--circuit', type=str, required=True,
                        help='zkllvm compiler output')
    parser.add_argument('-o', '--output', metavar='output file', type=str, required=True, help='output file')
    parser.add_argument('-n', '--name', type=str, required=True, help='name')
    args = parser.parse_args()

    data = {
        "name": "name",
        "description": "description",
        "url" : "url",
        "input_description": "input_description",
        "type": "type",
        "isPrivate": False,
        "definition": {
            "verification_key": "verification_key",
            "proving_key": "proofing_key"
        }
    }

    with open(args.circuit, 'r') as f:
        circuit = f.read()
        data['definition']['proving_key'] = circuit
    
    with open(args.output, 'w') as f:
        json.dump(data, f, indent=4)

    