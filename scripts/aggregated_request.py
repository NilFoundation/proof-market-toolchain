import argparse
import json
import shutil
import subprocess
import tempfile
import time
from datetime import datetime, timedelta


MERGE_TASK_KEY = 1234  # TODO: peplace with actual merge task key, adjust task input as well


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

    def __init__(self, scripts_path, subtasks_number, task_timeout, poll_interval):
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

    def distribute_and_merge_tasks(self, key, file, cost):
        completed_tasks = self._run_tasks(key, file, cost)
        return self._merge_proofs(completed_tasks, cost)

    def _push_task(self, key, file, cost, subkey=None):
        cmd = [
            "python3",
            f"{self.scripts_path}/request_tools.py",
            "push",
            "--cost", str(cost),
            "--file", file,
            "--key", str(key),
        ]
        if subkey is not None:
            cmd += ["--subkey", str(subkey)]
        result = self._run_command(cmd)
        response = self._extract_json(result)
        return response["_key"]

    def _get_proof(self, request_key):
        cmd = [
            "python3",
            f"{self.scripts_path}/proof_tools.py",
            "get",
            "--request_key", str(request_key),
        ]
        result = self._run_command(cmd)
        response = self._extract_json(result)
        return response["proof"]

    def _get_status(self, task_key):
        cmd = [
            "python3",
            f"{self.scripts_path}/request_tools.py",
            "get",
            "--key", str(task_key),
        ]
        result = self._run_command(cmd)
        response = self._extract_json(result)
        return response["status"]

    def _wait_for_completion(self, task_key):
        end_time = datetime.now() + timedelta(seconds=self.task_timeout)
        while datetime.now() < end_time:
            status = self._get_status(task_key)
            if status and status == "completed":
                return
            time.sleep(self.poll_interval)
        raise TimeoutError(f"Task {task_key} timed out.")

    def _process_level(self, tasks, cost):
        new_tasks = []
        i = 0
        while i < len(tasks):
            proofs = [self._get_proof(tasks[i])]
            i += 1

            if i < len(tasks):
                proofs.append(self._get_proof(tasks[i]))
                i += 1

            with tempfile.NamedTemporaryFile(mode="w") as tmp_file:
                json.dump(proofs, tmp_file)
                tmp_file.flush()
                combine_task_key = self._push_task(
                    MERGE_TASK_KEY, tmp_file.name, cost)

            new_tasks.append(combine_task_key)

        for task_key in progress_bar(new_tasks, prefix="Merges awaited:"):
            self._wait_for_completion(task_key)

        return new_tasks

    def _run_tasks(self, key, file, cost):
        tasks = [
            self._push_task(key, file, cost, i) for i in range(self.subtasks_number)
        ]
        for task_key in progress_bar(tasks, prefix="Proofs awaited:"):
            self._wait_for_completion(task_key)

        return tasks

    def _merge_proofs(self, tasks, cost):
        # Process results in a Merkle tree fashion
        while len(tasks) > 1:
            tasks = self._process_level(tasks, cost)

        return self._get_proof(tasks[0])


def main():
    parser = argparse.ArgumentParser(
        description="Distribute tasks and assemble results in a Merkle tree fashion."
    )
    parser.add_argument(
        "--scripts-path",
        required=True,
        help="Path to the directory containing the scripts",
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
        "--subtasks-number",
        type=int,
        required=True,
        help="How many subtasks to split into",
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
        args.scripts_path, args.subtasks_number, args.task_timeout, args.poll_interval
    )
    merged_proof = distributor.distribute_and_merge_tasks(
        args.key, args.file, args.cost
    )

    print(f"Final proof is: {merged_proof}")


if __name__ == "__main__":
    main()
