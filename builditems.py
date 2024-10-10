#! /usr/bin/env python
# encoding: utf-8
#
# Flare Global Item Manager
#

defines = {
  'default': ['FLARE=1'],
  'versal': ['FLARE_VERSAL'],
  'zynqmp': ['FLARE_ZYNQMP'],
  'zynq7000': ['FLARE_ZYNQ7000']
}

includes = {
  'default': [
    '.',
  ],
  'versal': [],
  'zynqmp': [],
  'zynq7000': []
}

cflags = {
  'default': ['-ffreestanding', '-g'],
  'versal': [],
  'zynqmp': [
        '-mcpu=cortex-a53',
        '-mfix-cortex-a53-835769',
        '-mfix-cortex-a53-843419',
        '-mlittle-endian',
        '-DEL3=1',
        '-DEL1_NONSECURE=0'
        '-mabi=lp64'
        ],
  'zynq7000': [
        '-march=armv7-a',
        '-mthumb',
        '-mfpu=neon',
        '-mfloat-abi=hard',
        '-mtune=cortex-a9',
        '-mlittle-endian',
        ]
}

def get_items(bld, items):
    vals = []
    vals += items['default']
    for board in items:
        if board == bld.env.FLARE_BOARD:
            vals += items[board]
    return vals

def get_defines(bld, items):
    vals = []
    vals += items['default']
    vals += defines['default']
    for board in items:
        if board == bld.env.FLARE_BOARD:
            vals += items[board]
            vals += defines[board]
    return vals

def get_includes(bld, items):
    vals = []
    vals += [str(bld.path.find_node(i)) for i in items['default']]
    vals += [str(bld.path.find_node(i)) for i in includes['default']]
    for board in items:
        if board == bld.env.FLARE_BOARD:
            vals += [str(bld.path.find_node(i)) for i in items[board]]
            vals += [str(bld.path.find_node(i)) for i in includes[board]]
    return vals

def get_cflags(bld, items):
    vals = []
    vals += items['default']
    vals += cflags['default']
    for board in items:
        if board == bld.env.FLARE_BOARD:
            vals += items[board]
            vals += cflags[board]
    return vals
