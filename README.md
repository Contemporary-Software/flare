# Flare FSBL

Flare is a small Apache 2.0 Licensed FSBL. It's a non-bricking light
weight bootloader designed for high reliability embedded systems.

Flare currently only supports booting U-Boot legacy images.

## Building

To build Flare configure providing the board, compiler prefix,
path to xsa (if required) and optionally a path to build tools.

```
./waf configure --tools-path=/opt/work/rtems/6/bin/ --compiler_prefix=arm-rtems6 --board=zynq7000 --xsa=~/mz7010_fmccc_2019_2/hardware/MZ7010_FMCCC_2019_2/MZ7010_FMCCC.xsa
```

After the build is configured you need to build
```
./waf
```
