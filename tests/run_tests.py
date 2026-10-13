#!/usr/bin/env python3
"""Run LLVM opt with the built plugin and verify transformed IR snippets."""

import argparse
import subprocess
import sys
from pathlib import Path


CASES = (
    ("constant-fold", "toolkit-const-fold"),
    ("local-cse", "toolkit-local-cse"),
    ("dead-code-elimination", "toolkit-dce"),
)


def verify_checks(check_file: Path, output: str) -> None:
    for line_number, raw_line in enumerate(
        check_file.read_text(encoding="utf-8").splitlines(), start=1
    ):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if line.startswith("CHECK-NOT:"):
            expected = line.removeprefix("CHECK-NOT:").strip()
            if expected in output:
                raise AssertionError(
                    f"{check_file.name}:{line_number}: unexpected IR: {expected}"
                )
        elif line.startswith("CHECK:"):
            expected = line.removeprefix("CHECK:").strip()
            if expected not in output:
                raise AssertionError(
                    f"{check_file.name}:{line_number}: missing IR: {expected}"
                )
        else:
            raise AssertionError(
                f"{check_file.name}:{line_number}: unsupported check directive"
            )


def run_case(opt: str, plugin: Path, tests_dir: Path, stem: str, pipeline: str) -> None:
    input_file = tests_dir / f"{stem}.ll"
    check_file = tests_dir / f"{stem}.check"
    command = [
        opt,
        f"-load-pass-plugin={plugin}",
        f"-passes={pipeline}",
        "-verify-each",
        "-S",
        str(input_file),
        "-o",
        "-",
    ]
    result = subprocess.run(
        command, check=False, capture_output=True, text=True, encoding="utf-8"
    )
    if result.returncode:
        raise RuntimeError(
            f"{' '.join(command)} failed with exit code {result.returncode}:\n"
            f"{result.stderr}"
        )
    verify_checks(check_file, result.stdout)
    print(f"PASS {stem} ({pipeline})")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--opt", required=True)
    parser.add_argument("--plugin", required=True, type=Path)
    parser.add_argument("--tests-dir", required=True, type=Path)
    args = parser.parse_args()

    for stem, pass_name in CASES:
        run_case(args.opt, args.plugin, args.tests_dir, stem, pass_name)
        run_case(args.opt, args.plugin, args.tests_dir, stem, "llvm-opt-toolkit")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, OSError, RuntimeError, subprocess.SubprocessError) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
