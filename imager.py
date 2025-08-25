#!/usr/bin/env python3
import sys
import zlib

RECORDS_OFFSET_ZYNQMP = 0x300000
RECORDS_OFFSET_ZYNQ7000 = 0x100000

if __name__ == "__main__":
    if '-h' in sys.argv or '--help' in sys.argv or len(sys.argv) <= 1:
        print("imager.py (Board) (BOOT.BIN) (Image) [output]")
        print("Supported boards: zynqmp, zynq7000")
        exit(0)
    board = sys.argv[1]
    bootbin = sys.argv[2]
    image_name = sys.argv[3]
    output = "flash.bin"
    if len(sys.argv) > 4:
        output = sys.argv[4]

    output_file = open(output, "wb")

    bootbin_file = open(bootbin, "rb")

    output_file.write(bootbin_file.read())

    bootbin_file.close()

    record_offset = 0
    if board == "zynqmp":
        record_offset = RECORDS_OFFSET_ZYNQMP
    elif board == "zynq7000":
        record_offset = RECORDS_OFFSET_ZYNQ7000
    else:
        print("Board not supported: supported boards: zynqmp, zynq7000")
        exit(1)

    output_file.seek(record_offset)

    file = open(image_name, "rb")
    data = file.read()
    data_crc = zlib.crc32(data)

    record = bytearray(len(data).to_bytes(4, 'little'))
    record.extend(format(data_crc, '08x').encode('ascii'))

    length = 12 + 12 * 2

    header = bytearray(length.to_bytes(4, 'little'))
    header.extend(record)
    while len(header) < (length - 8):
        header.append(0)
    header_crc = zlib.crc32(header)

    output_file.write(format(header_crc, 'x').encode('ascii'))
    output_file.write(header)
    output_file.write(data)
