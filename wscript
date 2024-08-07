#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'flare'
VERSION = '0.1'

bsps = {}
bsps['aarch64'] = [
        'versal',
        'zynqmp',
        ]
bsps['arm'] = [
        'zynq7000',
        ]

common_files = [
        '_exit.c',
        'boot-buffer.c',
        'boot-factory-config.c',
        'boot-filesystem.c',
        'boot-load.c',
        'boot-script.c',
        #'datasafe-ssbl.c',
        'datasafe.c',
        'factory-boot.c',
        'flash.c',
        'fsbl-boot.c',
        'gpio.c',
        'inbyte.c',
        'libc.c',
        #'main.c',
        'jffs2-boot/libcrc32.c',
        'jffs2-boot/jffs2-boot.c',
        'md5.c',
        'outbyte.c',
        'printf.c',
        'reset.c',
        #'ssbl-boot.c',
        #'test.c',
        'tzlib.c',
        'usleep.c',
        ]

aarch64_files = [
        'aarch64-cache.c',
        'aarch64-error.c',
        'aarch64-fsbl-boot-start.S',
        'aarch64-handoff.c',
        'aarch64-handoff.S',
        'aarch64-mmu/aarch64-cache.c',
        'empty-wdog.c',
        'wdog-empty.c',
        ]

versal_files = [
        'aarch64-mmu/versal-mmu.c',
        'empty-leds.c',
        'empty-power-switch.c',
        'empty-slcr.c',
        'versal-flash.c',
        'versal-timer.c',
        'versal-uart.c',
        'versal_vectors.S',
        ]

zynqmp_files = [
        'aarch64-mmu/zynqmp-mmu.c',
        'empty-leds.c',
        'empty-power-switch.c',
        'empty-slcr.c',
        'zynqmp-timer.c',
        'zynqmp-flash.c',
        'zynqmp-factory-boot.c',
        'zynqmp-uart.c',
        'zynqmp-vectors.S',
        'zynqmp-pre-main.c',
        'zynqmp-atf.c',
        'zynqmp-atf-return.S',
        ]

zynq7000_files = [
        'arm-fsbl-boot-start.S',
        'wdog-mio30-mio31.c',
        'zynq7000-handoff.c',
        'zynq7000-slcr.c',
        'zynq7000-timer.c',
        'zynq7000-uart.c',
        'zynq7000-cache.c',
        'zynq7000-error.c',
        'zynq7000-flash.c',
        'zynq7000-leds.c',
        'zynq7000-mmu.c',
        'zynq7000-power-switch.c',
        'zynq7000-wdog.c',
        ]


files = {}
files['versal'] = [
        common_files,
        aarch64_files,
        versal_files,
        ]
files['zynq7000'] = [
        common_files,
        zynq7000_files,
        ]
files['zynqmp'] = [
        common_files,
        aarch64_files,
        zynqmp_files,
        ]

def bsplist(ctx):
    print('BSPs:')
    for arch in sorted(bsps):
        for bsp in sorted(bsps[arch]):
            variant = arch + "/" + bsp
            print(variant)

def options(ctx):
    ctx.add_option('--arch', action='store', default='aarch64', help='Architecture to cross compile')
    ctx.add_option('--bsp', action='store', default='versal', help='Board to be build')
    ctx.add_option('--rtemstools', action='store', default=False, help='Path to RTEMS tools')
    ctx.add_option('--xsa', action='store', default=False, help='Path to XSA')

def configure(ctx):
    print('Arch: ' + ctx.options.arch)
    print('BSP: ' + ctx.options.bsp)
    print('RTEMS tools: ' + ctx.options.rtemstools)

    rtemstoolsbin = ctx.options.rtemstools + '/bin'
    ctx.env.FLARE_BSP = ctx.options.bsp
    ctx.env.FLARE_ARCH = ctx.options.arch
    ctx.env.FLARE_XSA = ctx.options.xsa

    ctx.find_program(ctx.options.arch + '-rtems6-gcc', path_list=rtemstoolsbin, var="CC")

def build(bld):

    bld(rule='unzip -d ${TGT} ${SRC}',
        source=bld.root.find_node(bld.env.FLARE_XSA),
        target=bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/xsa/open')

    bld.add_group()

    bld.env.INCLUDES = ['-I../bootloader', '-I' + bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/xsa/open/']
    arch_flag = '-DFLARE_' + bld.env.FLARE_ARCH.upper() + '_' + bld.env.FLARE_BSP.upper() 
    bld.env.CFLAGS=['-Wall', '-ffreestanding', '-g', '-DEL3=1 -DEL1_NONSECURE=0', '-flto', arch_flag]
    bld.env.LINKER_FLAGS = '-nostdlib -nolibc -nodefaultlibs -ffreestanding -flto'
    if bld.env.FLARE_BSP == 'zynqmp':
        bld.env.CFLAGS.append('-mcpu=cortex-a53 -mfix-cortex-a53-835769 -mfix-cortex-a53-843419 -mlittle-endian -mabi=lp64')
    linker_files = []
    for fileset in files[bld.env.FLARE_BSP]:
        for file in fileset:
            bld(
                rule='${CC} ${CFLAGS} ${INCLUDES} -c ${SRC} -o ${TGT}',
                target=bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/bootloader/' + file + '.o',
                source='bootloader/' + file)
            linker_files.append(bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/bootloader/' + file + '.o')

    bld(
        rule='${CC} ${CFLAGS} ${INCLUDES} -c ${SRC} -o ${TGT}',
        target=bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/xsa/psu_init.c.o',
        source='build/' + bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/xsa/open/psu_init.c')
    linker_files.append(bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/xsa/psu_init.c.o')

    bld.add_group()
    bld(rule='${CC} ' + " ".join(linker_files) + ' ${LINKER_FLAGS} -T' +
        ' ../bootloader/${FLARE_BSP}-lscript.ld -o ${FLARE_ARCH}/${FLARE_BSP}/flare_fsbl.elf',
        source=bld.env.FLARE_ARCH + '/' + bld.env.FLARE_BSP + '/bootloader/fsbl-boot.c.o')


