#
# run.sh --> compiles AND runs the main.cpp file in one go, and hides .o files on completion
#

#!/bin/bash

set -e
g++ main.cpp -o main -le101; 
sudo ./main;


