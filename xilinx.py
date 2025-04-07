import os
import shutil

outputs = {
    'default': [],
    'versal': [],
    'zynqmp': [],
    'zynq7000': [
        'xilinx/ps7_init.c',
        ]
}

def build(bld):
    from waflib import Task

    def unzip_xsa(task):
        board = task.env.FLARE_BOARD
        path = task.env.FLARE_XSA
        if path:
            if board == 'zynq7000':
                shutil.unpack_archive(path, 'build/xsa', 'zip')
                task.env.FLARE_PS_INIT_SRC = 'build/xsa/ps7_init.c'
        else:
            task.env.FLARE_PS_INIT_SRC = task.env.FLARE_PS_INIT

    def copy_ps_init(task):
        path = task.env.FLARE_PS_INIT_SRC
        output = str(task.outputs[0])
        if not os.path.exists('build/xilinx'):
            os.mkdir('build/xilinx')
        shutil.copyfile(path, output)


    class xilinx_init(Task.Task):
        color = 'CYAN'
        always_run = True
        run_str = (unzip_xsa, copy_ps_init)

    board = bld.env.FLARE_BOARD
    outs = [bld.path.find_or_declare(file) for file in outputs[board]]

    xilinx_init_tsk = xilinx_init(env=bld.env)
    xilinx_init_tsk.set_outputs(outs)
    bld.add_to_group(xilinx_init_tsk)
