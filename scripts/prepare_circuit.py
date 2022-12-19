import json
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--circuit', type=str, required=True,
                        help='zkllvm compiler output')
    parser.add_argument('-o', '--output', metavar='output file', type=str, required=True, help='output file')
    args = parser.parse_args()

    data = {
        "name": "name",
        "type": "type",
        "description": "description",
        "definition_url": "definition_url",
        "verification_key": "verification_key",
        "statement": "statement"
    }

    with open(args.circuit, 'r') as f:
        circuit = f.read()
        data['statement'] = circuit
    
    with open(args.output, 'w') as f:
        json.dump(data, f, indent=4)

    