import os
import shutil
import argparse


if __name__ == '__main__':
    parser = argparse.ArgumentParser("Copy src file to dst path")
    parser.add_argument('--src', type=str, required=True, help='path to src file')
    parser.add_argument('--dst', type=str, required=True, help='path to dst')
    args = parser.parse_args()
    if not os.path.exists(args.src):
        raise FileNotFoundError(args.src)
    if os.path.isdir(args.dst) and not os.path.exists(args.dst):
        os.makedirs(args.dst)
    shutil.copy(args.src, args.dst)