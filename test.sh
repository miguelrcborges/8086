#!/bin/bash

cd tests || { echo "Could not find tests folder. Exiting..."; exit 1; }

for asm_file in *.asm; do
    base_name="${asm_file%.*}"

    nasm -o "${base_name}_og.bin" "$asm_file" || { echo "Failed to compile $asm_file."; continue; }

    ../disass "${base_name}_og.bin" > "${base_name}_dis.asm"

    nasm -o "${base_name}_dis.bin" "${base_name}_dis.asm" || { echo "Failed to compile disassembled output of $asm_file."; continue; }

    original_sha=$(sum "${base_name}_og.bin" | cut -d ' ' -f 1)
    disassemble_sha=$(sum "${base_name}_dis.bin" | cut -d ' ' -f 1)

    if [[ "$original_sha" == "$disassemble_sha" ]]; then
        echo "$asm_file PASSED"
    else
        echo "$asm_file FAILED"
    fi

    rm -f "${base_name}_og.bin" "${base_name}_dis.asm" "${base_name}_dis.bin"
done

cd -
