#!/bin/bash

cd build
source models-venv/bin/activate
cd ../server
python3 server.py
