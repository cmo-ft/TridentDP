# TRIDENT Data Process
Data process chain of TRIDENT based on bamboo-shoot3 and ROOT.

* Do not support c++17

The repository contains 2 parts: `Emulator` and `Data Process`.

## Emulator
Transform the `data.root` (output of the `hDom` program) into the 
data format of PMT readout.

The output, `waveform.dat`, is written in binary and 
can be changed into text format with the `raw_reader` in `src/`.


## Data Process
This is the main part of the repository. 

`raw_to_group` can transform the `.dat` file into the `.bsd` format
which is easy for storage and transfer.

`group_to_root` can transform the `.bsd` file back into `.root` file 
and save the waveform.