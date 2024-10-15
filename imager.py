#!/usr/bin/env python3
import sys
import hashlib
import gzip

RECORDS_OFFSET_ZYNQMP = 0x300000
RECORDS_OFFSET_ZYNQ7000 = 0x100000

if __name__ == "__main__":
    if '-h' in sys.argv or '--help' in sys.argv or len(sys.argv) <= 1:
        print("imager.py (Board) (BOOT.BIN) (config) [output]")
        print("Supported boards: zynqmp, zynq7000")
        exit(0)
    board = sys.argv[1]
    bootbin = sys.argv[2]
    config_name = sys.argv[3]
    output = "flash.bin"
    if len(sys.argv) > 4:
        output = sys.argv[4]

    config_file = open(config_name)

    output_file = open(output, "wb")

    bootbin_file = open(bootbin, "rb")

    output_file.write(bootbin_file.read())

    bootbin_file.close()

    record_offset = 0;
    if board == "zynqmp":
        record_offset = RECORDS_OFFSET_ZYNQMP
    elif board == "zynq7000":
        record_offset = RECORDS_OFFSET_ZYNQ7000
    else:
        print("Board not supported: supported boards: zynqmp, zynq7000")
        exit(1)

    output_file.seek(record_offset)

    records = []
    records_data = []

    for record_data in config_file:
        items = record_data.split(",")
        print(record_data)
        file_name = items[0]
        file = items[1]
        base_addr = int(items[2], 16)
        start_addr = int(items[3], 16)

        file = open(file, "rb")
        data = gzip.compress(file.read(), compresslevel=9, mtime=0)
        data_md5 = hashlib.md5(data)

        record = bytearray(file_name, 'ascii');
        if len(record) > 32:
            print("File name too long: " + file_name)
        else:
            while (len(record) < 32):
                record.append(0)
        record.extend(len(data).to_bytes(4, 'little'))
        record.extend(base_addr.to_bytes(4, 'little'))
        record.extend(start_addr.to_bytes(4, 'little'))
        record.extend(data_md5.digest())
        records.append(record)
        records_data.append(data);

    if len(records) > 2:
        print("Too many records. Only 2 supported:")

    length = 20 + 60 * 2

    header = bytearray(length.to_bytes(4, 'little'))
    for record in records:
        header.extend(record)
    while len(header) < (length - 16):
        header.append(0)
    records_md5 = hashlib.md5(header)

    output_file.write(records_md5.digest())
    output_file.write(header)
    for data in records_data:
        output_file.write(data)
