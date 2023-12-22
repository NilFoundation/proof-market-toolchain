import argparse
import json
import shutil
import subprocess
import tempfile
import time
from datetime import datetime, timedelta
from request_tools import get as get_request
from request_tools import push as push_request


INTERMEDIATE_LAYER_VERIFIER_TASK = 1234  # TODO: peplace with actual merge task key, adjust task input as well
ROOT_VERIFIER_TASK = 4321  # TODO: peplace with actual merge task key, adjust task input as well


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


class TaskDistributor:
    class TimeoutError(Exception):
        pass

    def __init__(self, auth, scripts_path, subtasks_number, task_timeout, poll_interval):
        self.auth = auth
        self.scripts_path = scripts_path
        self.subtasks_number = subtasks_number
        self.task_timeout = task_timeout
        self.poll_interval = poll_interval

    @staticmethod
    def _extract_json(text):
        # proof_tools output contains multiple JSON's, we need the last one
        try:
            json_str = text[text.rindex("{"): text.rindex("}") + 1]
            return json.loads(json_str)
        except (ValueError, json.JSONDecodeError):
            return None

    @staticmethod
    def _run_command(command):
        result = subprocess.run(command, stderr=subprocess.PIPE, text=True)
        return result.stderr

    def distribute_and_merge_tasks(self, key, input_file, cost):
        completed_tasks = self._order_layer0_tasks(key, input_file, cost)
        return self._merge_proofs(completed_tasks, cost)

    def _push_task(self, statement_key, input_file, cost, aggregated_mode_id):
        response = push_request(self.auth, statement_key, input_file, cost, aggregated_mode_id=aggregated_mode_id, verbose=True)
        return response["_key"]["id"]

    def _get_proof(self, request_key):
        response = get_request(self.auth, request_key, verbose=True)
        return response["proof"]

    def _get_status(self, request_key):
        response = get_request(self.auth, request_key, verbose=True)
        return response["status"]

    def _wait_for_completion(self, request_key):
        end_time = datetime.now() + timedelta(seconds=self.task_timeout)
        while datetime.now() < end_time:
            status = self._get_status(request_key)
            if status and status == "DONE":
                return
            time.sleep(self.poll_interval)
        raise TimeoutError(f"Task {request_key} timed out.")

    def _process_intermediate_layer(self, tasks, cost):
        new_tasks = []
        i = 0
        while i < len(tasks):
            proofs = [self._get_proof(tasks[i])]
            i += 1

            if i < len(tasks):
                proofs.append(self._get_proof(tasks[i]))
                i += 1

            with tempfile.NamedTemporaryFile(mode="w") as joined_proofs_file:
                json.dump(proofs, joined_proofs_file)
                joined_proofs_file.flush()
                combine_task_key = self._push_task(
                    INTERMEDIATE_LAYER_VERIFIER_TASK, joined_proofs_file.name, cost)

            new_tasks.append(combine_task_key)

        for request_key in progress_bar(new_tasks, prefix="Intermediate layers awaited:"):
            self._wait_for_completion(request_key)

        return new_tasks

    def _order_layer0_tasks(self, key, input_file, cost):
        tasks = [
            self._push_task(key, input_file, cost, aggregated_mode_id) for aggregated_mode_id in range(self.subtasks_number)
        ]
        for request_key in progress_bar(tasks, prefix="Proofs awaited:"):
            self._wait_for_completion(request_key)

        return tasks

    def _merge_proofs(self, tasks, cost):
        # Process results in a binary tree fashion
        while len(tasks) > 1:
            tasks = self._process_layer(tasks, cost)

        return self._get_proof(tasks[0])


def main():
    parser = argparse.ArgumentParser(
        description="Distribute tasks and assemble results in a Merkle tree fashion."
    )
    parser.add_argument("--auth", type=str, help="auth file")
    parser.add_argument(
        "--scripts-path",
        required=True,
        help="Path to the directory containing the scripts",
    )
    parser.add_argument(
        "--statement-key", required=True, help="Key to be forwarded to request_tool.py"
    )
    parser.add_argument(
        "--input", required=True, help="Input file for the proof producer"
    )
    parser.add_argument(
        "--cost",
        type=int,
        required=True,
        help="Cost parameter for proof producing",
    )
    parser.add_argument(
        "--aggregation-ratio",
        type=int,
        required=True,
        help="How many provers to use on the zero layer",
    )
    parser.add_argument(
        "--task-timeout",
        type=int,
        default=120,
        help="Timeout in seconds for while waiting for task to complete",
    )
    parser.add_argument(
        "--poll-interval", type=int, default=5, help="Long polling interval in seconds"
    )

    args = parser.parse_args()

    distributor = TaskDistributor(
        args.auth, args.scripts_path, args.aggregation_ratio, args.task_timeout, args.poll_interval
    )
    merged_proof = distributor.distribute_and_merge_tasks(
        args.statement_key, args.input, args.cost
    )

    print(f"Final proof is: {merged_proof}")


if __name__ == "__main__":
    main()
