import argparse
import json
import shutil
import subprocess
import tempfile
import time
from datetime import datetime, timedelta


MERGE_TASK_KEY = 1234


def progress_bar(iterable, prefix="", suffix="", fill="█"):
    total = len(iterable)

    def print_bar(iteration):
        columns, _ = shutil.get_terminal_size(fallback=(80, 20))
        length = columns - len(prefix) - len(suffix) - 10
        percent = ("{0:.1f}").format(100 * (iteration / float(total)))
        filled_length = int(length * iteration // total)
        bar = fill * filled_length + "-" * (length - filled_length)
        print(f"\r{prefix} |{bar}| {percent}% {suffix}", end="\r")
        if iteration == total:
            print()

    print_bar(0)

    for i, item in enumerate(iterable, 1):
        yield item
        print_bar(i)


class TimeoutError(Exception):
    pass


def dummy_num_parts(key):
    # TODO: extract parallel factor from .ll file
    return 2


def extract_json(text):
    try:
        json_str = text[text.index("{"): text.rindex("}") + 1]
        return json.loads(json_str)
    except (ValueError, json.JSONDecodeError):
        return None


def push_task(key, file, cost, subkey=None):
    cmd = [
        "python3", "request_tools.py", "push",
        "--cost", str(cost),
        "--file", file,
        "--key", str(key),
    ]
    if subkey is not None:
        cmd += ["--subkey", str(subkey)]
    result = subprocess.run(cmd, stdout=subprocess.PIPE)
    response = extract_json(result.stdout.decode())
    return response["_key"] if response and "_key" in response else None


def get_proof(request_key):
    cmd = ["python3", "proof_tools.py", "get",
           "--request_key", str(request_key)]
    result = subprocess.run(cmd, stdout=subprocess.PIPE)
    return result.stdout.decode().strip()


def get_status(task_key):
    cmd = ["python3", "request_tools.py", "get", "--key", str(task_key)]
    result = subprocess.run(cmd, stdout=subprocess.PIPE)
    return extract_json(result.stdout.decode())


def wait_for_completion(task_key, timeout, poll_interval):
    end_time = datetime.now() + timedelta(seconds=timeout)
    while datetime.now() < end_time:
        status = get_status(task_key)
        if status and status["status"] == "completed":
            return status
        time.sleep(poll_interval)
    raise TimeoutError(f"Task {task_key} timed out.")


def run_tasks(key, file, cost, task_timeout, poll_interval):
    num_parts = dummy_num_parts(key)
    tasks = [push_task(key, file, cost, i) for i in range(num_parts)]
    for task_key in progress_bar(tasks, prefix="Proofs awaited:"):
        wait_for_completion(task_key, task_timeout, poll_interval)

    return tasks


def process_level(tasks, cost, task_timeout, poll_interval):
    new_tasks = []
    i = 0
    while i < len(tasks):
        proofs = [get_proof(tasks[i])]
        i += 1

        if i < len(tasks):
            proofs.append(get_proof(tasks[i]))
            i += 1

        with tempfile.NamedTemporaryFile(mode="w") as tmp_file:
            json.dump(proofs, tmp_file)
            tmp_file.flush()
            combine_task_key = push_task(MERGE_TASK_KEY, tmp_file.name, cost)

        new_tasks.append(combine_task_key)

    for task_key in progress_bar(new_tasks, prefix="Merges awaited:"):
        wait_for_completion(task_key, task_timeout, poll_interval)

    return new_tasks


def merge_proofs(tasks, cost, task_timeout, poll_interval):
    # Process results in a Merkle tree fashion
    while len(tasks) > 1:
        tasks = process_level(tasks, cost, task_timeout, poll_interval)

    return get_proof(tasks[0])


def main():
    parser = argparse.ArgumentParser(
        description="Distribute tasks and assemble results in a Merkle tree fashion."
    )
    parser.add_argument(
        "--key", required=True, help="Key to be forwarded to request_tool.py"
    )
    parser.add_argument(
        "--file", required=True, help="File to be forwarded to request_tool.py"
    )
    parser.add_argument(
        "--cost",
        type=int,
        required=True,
        help="Cost parameter to be forwarded to request_tool.py",
    )
    parser.add_argument(
        "--task-timeout",
        type=int,
        required=True,
        help="Timeout in seconds for while waiting for task to complete",
    )
    parser.add_argument(
        "--poll-interval", type=int, default=5, help="Long polling interval in seconds"
    )

    args = parser.parse_args()

    completed_tasks = run_tasks(
        args.key, args.file, args.cost, args.task_timeout, args.poll_interval
    )
    merged_proof = merge_proofs(
        completed_tasks, args.cost, args.task_timeout, args.poll_interval
    )

    print(f"Final proof is: {merged_proof}")


if __name__ == "__main__":
    main()
