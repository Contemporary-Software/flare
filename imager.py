#!/usr/bin/env python3
import sys
import hashlib
import gzip

RECORDS_OFFSET = 0x300000

if __name__ == "__main__":
    if '-h' in sys.argv or '--help' in sys.argv or len(sys.argv) <= 1:
        print("imager.py (BOOT.BIN) (config) [output]")
        exit(0)
    bootbin = sys.argv[1]
    config_name = sys.argv[2]
    output = "flash.bin"
    if len(sys.argv) > 3:
        output = sys.argv[3]

    config_file = open(config_name)

    output_file = open(output, "wb")

    bootbin_file = open(bootbin, "rb")

    output_file.write(bootbin_file.read())

    bootbin_file.close()

    output_file.seek(RECORDS_OFFSET)

    records = []
    records_data = []

    for record_data in config_file:
        items = record_data.split(",")
        print(record_data)
        file_name = items[0]
        file = items[1]
        addr = int(items[2], 16)

        file = open(file, "rb")
        data = gzip.compress(file.read())
        data_md5 = hashlib.md5(data)

        record = bytearray(file_name, 'ascii');
        if len(record) > 32:
            print("File name too long: " + file_name)
        else:
            while (len(record) < 32):
                record.append(0)
        record.extend(len(data).to_bytes(4, 'little'))
        record.extend(addr.to_bytes(4, 'little'))
        record.extend(data_md5.digest())
        records.append(record)
        records_data.append(data);

    length = 20 + 56 * len(records)

    header = bytearray(length.to_bytes(4, 'little'))
    for record in records:
        header.extend(record)
    records_md5 = hashlib.md5(header)

    output_file.write(records_md5.digest())
    output_file.write(header)
    for data in records_data:
        output_file.write(data)
