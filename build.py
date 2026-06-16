#!/usr/bin/env python3

import hashlib
import json
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from threading import Thread
from typing import Any

ROOT_PATH = Path(__file__).parent
SRC_PATH = ROOT_PATH / "src"
BUILD_PATH = ROOT_PATH / "build"
OBJ_PATH = BUILD_PATH / "obj"
HASH_PATH = BUILD_PATH / "hash.json"
EXECUTABLE_PATH = BUILD_PATH / "rlzip"
SOURCE_SUFFIXES = [".cpp", ".cxx", ".cc", ".C", ".c++", ".cp"]
HEADER_SUFFIXES = [".h", ".hpp"]


class JsonReader:
    @staticmethod
    def write(obj: Any, path: Path) -> None:
        with open(path, mode="w", encoding="utf-8") as json_file:
            json.dump(obj, json_file, indent=4)

    @staticmethod
    def read(path: Path) -> Any:
        if not path.exists():
            return {}

        with open(path, encoding="utf-8") as json_file:
            file = json.load(json_file)

        return file


@dataclass
class Args:
    params: list[str] = field(default_factory=list)
    force: bool = False
    exec: bool = False
    debug: bool = False


def get_args() -> Args:
    args = Args()
    args_list = sys.argv[1:] if len(sys.argv) > 1 else []

    for arg in args_list:
        if arg == "--force":
            args.force = True
        if arg == "--exec":
            args.exec = True
        if arg == "--debug":
            args.debug = True
        else:
            args.params.append(arg)

    return args


def get_file_hash(file: Path) -> str:
    buf_size = 65536
    sha512 = hashlib.sha512()

    with open(file, "rb") as f:
        while True:
            data = f.read(buf_size)
            if not data:
                break
            sha512.update(data)

    return sha512.hexdigest()


def include_sources(
    sources: dict[str, Path], headers: dict[str, Path], dir: Path
) -> tuple[dict[str, Path], dict[str, Path]]:
    for entity in dir.iterdir():
        if entity.is_dir():
            include_sources(sources, headers, entity)
        elif entity.is_file() and entity.suffix in SOURCE_SUFFIXES:
            encoded_path = str(entity).encode()
            hash = hashlib.sha512(encoded_path)
            sources[hash.hexdigest()] = entity
        elif entity.is_file() and entity.suffix in HEADER_SUFFIXES:
            encoded_path = str(entity).encode()
            hash = hashlib.sha512(encoded_path)
            headers[hash.hexdigest()] = entity

    return sources, headers


def calculate_hashes(files: dict[str, Path], debug: bool = False):
    def __target(file: Path, hashes: dict) -> None:
        hash = get_file_hash(file)
        if debug:
            print(f"{file} - complited hash")
        hashes[file] = hash

    hashes = {}
    threads: list[Thread] = []
    for _, path in files.items():
        t = Thread(target=__target, args=(path, hashes))
        t.start()
        threads.append(t)

    for t in threads:
        t.join()

    return hashes


def need_invalidation(headers: dict[str, Path], hashes: dict[Path, str]) -> bool:
    need = False
    file_hashes: dict[str, str] = JsonReader.read(HASH_PATH)

    for hash, path in headers.items():
        file_hash = file_hashes.get(hash)
        current_file_hash = hashes[path]
        if file_hash is None or file_hash != current_file_hash:
            file_hashes[hash] = current_file_hash
            need = True

    JsonReader.write(file_hashes, HASH_PATH)

    return need


def update_headers_hashes(headers: dict[str, Path], hashes: dict[Path, str]):
    file_hashes: dict[str, str] = JsonReader.read(HASH_PATH)

    for hash, path in headers.items():
        current_file_hash = hashes[path]
        file_hashes[hash] = current_file_hash

    JsonReader.write(file_hashes, HASH_PATH)


def invalidate_hashes():
    JsonReader.write({}, HASH_PATH)


def compile_objects(object_paths: dict[Path, Path]):
    def __target(path: Path, obj_path: Path) -> None:
        result = subprocess.run(["g++", "-std=c++23", "-c", path, "-o", obj_path])
        if result.returncode != 0:
            raise RuntimeError

    threads: list[Thread] = []
    for path, obj_path in object_paths.items():
        t = Thread(target=__target, args=(path, obj_path))
        t.start()
        threads.append(t)

    for t in threads:
        t.join()


def assemble_sources(sources: dict[str, Path], hashes: dict[Path, str]) -> int:
    changes = 0

    object_paths: dict[Path, Path] = {}
    file_hashes: dict[str, str] = JsonReader.read(HASH_PATH)

    for hash, path in sources.items():
        file_hash = file_hashes.get(hash)
        current_file_hash = hashes[path]
        if file_hash is None or file_hash != current_file_hash:
            file_hashes[hash] = current_file_hash
            obj_path = OBJ_PATH / f"{hash}.o"
            object_paths[path] = obj_path
            changes += 1

    compile_objects(object_paths)
    JsonReader.write(file_hashes, HASH_PATH)

    return changes


def link_objects(sources: dict[str, Path], changes: int) -> None:
    if changes == 0:
        return None

    objects: list[Path] = []
    for hash in sources.keys():
        obj_path = OBJ_PATH / f"{hash}.o"
        objects.append(obj_path)

    subprocess.run(["g++", *objects, "-o", EXECUTABLE_PATH])


def run(args: list[str]) -> None:
    try:
        subprocess.run([EXECUTABLE_PATH, *args])
    except FileNotFoundError:
        print("Error: executable file was not found due to error during compilation")
    except KeyboardInterrupt:
        return


def build() -> None:
    args = get_args()

    BUILD_PATH.mkdir(exist_ok=True)
    OBJ_PATH.mkdir(exist_ok=True)
    if not HASH_PATH.exists():
        JsonReader.write({}, HASH_PATH)

    sources, headers = include_sources({}, {}, SRC_PATH)
    source_hashes = calculate_hashes(sources, args.debug)
    header_hashes = calculate_hashes(headers, args.debug)
    is_invalidation_needed = need_invalidation(headers, header_hashes)

    if args.exec:
        run(args.params)
        return

    if args.force or is_invalidation_needed:
        invalidate_hashes()
        update_headers_hashes(headers, header_hashes)

    try:
        changes = assemble_sources(sources, source_hashes)
    except RuntimeError:
        return

    link_objects(sources, changes)

    run(args.params)


if __name__ == "__main__":
    build()
