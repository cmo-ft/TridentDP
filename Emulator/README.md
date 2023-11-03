# Waveform Simulation
usage:
```shell
WaveformSimu data.root output.dat waveform_template.yaml
```

## Output file format
The output file is designed to store various data packets, with each packet representing a triggered PMT.
As of the current configuration, each data packet includes the following information:

```
protocol, channel_id, event_id / time_stamp, v0, v1, ..., v2499
```

Here's a breakdown of each field:

    protocol: This field is fixed at 0xb1 (hexadecimal) or 177 (decimal) and signifies the protocol used.

    channel_id: This field is calculated as DomId * 100 + PmtId. It uniquely identifies the source of the data.

    event_id / time_stamp: This field contains either the event ID or a timestamp, depending on the specific context. It provides information about when the data was recorded.

    v0, v1, ..., v2499: These fields represent the voltage values for each of the 2500 bins. Each bin corresponds to a time interval of 2ns.

Consequently, the total length of each data packet is 2503, including all the mentioned fields and their respective values. 