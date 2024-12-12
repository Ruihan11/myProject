#!/bin/bash

# Default Verilog file name
VERILOG_FILE="example.v"
TB="testbench.v"
VCD_FILE="waveform.vcd"
OUTPUT_FILE="sim_output"

# Compile the Verilog file
echo "Compiling Verilog file..."
iverilog -o "$OUTPUT_FILE" "$VERILOG_FILE" "$TB"
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check for errors in the Verilog code."
    exit 1
fi
echo "Compilation successful!"

# Run the simulation to generate the .vcd file
echo "Running simulation..."
vvp "$OUTPUT_FILE"
if [ $? -ne 0 ]; then
    echo ".vcd simulation failed. Please check for errors in the Verilog code."
    exit 1
fi
echo "Simulation complete. Generated waveform file: $VCD_FILE"

# GTKWave
if [ -f "$VCD_FILE" ]; then
    echo "Opening GTKWave to view the waveform..."
    gtkwave "$VCD_FILE"
else
    echo "Waveform file $VCD_FILE not found. Please ensure the simulation generated a .vcd file."
fi

rm -f $OUTPUT_FILE
