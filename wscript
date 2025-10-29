#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'flare'
VERSION = '0.1'

import buildcontrol
import builditems
import buildver
import xilinx

directories = ['bootloader']

sources = {
    'default': [
        'bootloader/fsbl-boot.c',
        'bootloader/flare-build-id.c',
    ],
    'versal': [],
    'zynqmp': [
        'xilinx/psu_init.c',
    ],
    'zynq7000': ['xilinx/ps7_init.c', 'bootloader/zynq7000_vectors.S']
}

includes = {
    'default': [
        'bootloader',
        'bootloader/drivers',
    ],
    'versal': [],
    'zynqmp': ['bootloader/drivers/timer'],
    'zynq7000': []
}

defines = {'default': [], 'versal': [], 'zynqmp': [], 'zynq7000': []}

cflags = {'default': [], 'versal': [], 'zynqmp': [], 'zynq7000': []}

linkflags = {
    'default': [],
    'versal': [],
    'zynqmp': ['-T../bootloader/zynqmp-lscript.ld'],
    'zynq7000': ['-T../bootloader/zynq7000-lscript.ld']
}


def init(ctx):
    buildcontrol.recurse(ctx, directories)


def options(opt):
    buildcontrol.recurse(opt, directories)
    buildcontrol.options(opt)


def configure(conf):
    buildcontrol.recurse(conf, directories)
    buildcontrol.configure(conf)


def build(bld):
    buildver.build(bld)
    xilinx.build(bld)
    buildcontrol.recurse(bld, directories)
    bld.program(target='flare_fsbl',
                features='c cprogram',
                linkflags=builditems.get_cflags(bld, linkflags),
                cflags=builditems.get_cflags(bld, cflags),
                asflags=builditems.get_cflags(bld, cflags),
                includes=builditems.get_includes(bld, includes),
                defines=builditems.get_defines(bld, defines),
                source=builditems.get_items(bld, sources),
                use=['flare', 'flare_drivers'],
                install_path='${PREFIX}/share/flare/${FLARE_BOARD}')
