#!/bin/bash
cd input/
rm -f PerformanceMetrics.txt FS_DMEMResult.txt FS_RFResult.txt StateResult_FS.txt SS_DMEMResult.txt SS_RFResult.txt StateResult_SS.txt

cd ..
rm -f main.elf

g++ -o main.elf main.cpp
sleep 1

if [ $? -eq 0 ]; then
    echo "Running ..."
    ./main.elf
    echo "Done"
else
    echo "Failed to compile"
fi

