#/bin/bash

# Old version, deprecated
#python3 setup.py build
#sudo python3 setup.py install

# New version
python3 -m venv .venv
source .venv/bin/activate
python3 -m build
pip install .
