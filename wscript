#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'flare'
VERSION = '0.1'

import buildcontrol
import builditems
import xsa

directories = ['bootloader']

sources = {
    'default': [
        'bootloader/fsbl-boot.c',
      ],
    'versal': [],
    'zynqmp': [
        'build/xsa/psu_init.c',
        ],
    'zynq7000': []
}

includes = {
    'default': [],
    'versal': [],
    'zynqmp': [
        'bootloader',
        'bootloader/drivers/io',
        'bootloader/drivers/timer',
        'bootloader/drivers/uart',
        'bootloader/drivers/flash',
        'bootloader/drivers/leds',
        'bootloader/drivers/md5',
        'bootloader/drivers/power-switch',
        'bootloader/drivers/wdog',
        'bootloader/drivers/slcr',
    ],
    'zynq7000': []
}

defines = {
    'default': [],
    'versal': [],
    'zynqmp': [],
    'zynq7000': []
}

cflags = {
    'default': [],
    'versal': [],
    'zynqmp': [],
    'zynq7000': []
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
    if bld.env.FLARE_XSA:
        xsa.unzip_xsa(bld.env.FLARE_XSA)
    buildcontrol.recurse(bld, directories)
    bld.program(target='flare_fsbl',
                features='c cprogram',
                linkflags='-T ../bootloader/zynqmp-lscript.ld',
                includes=builditems.get_includes(bld, includes),
                defines=builditems.get_defines(bld, defines),
                source=builditems.get_items(bld, sources),
                use=['flare', 'flare_drivers'])
