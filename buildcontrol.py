#
# Flare Build Control
#

import os

def recurse(ctx, directories):
    for d in directories:
        ctx.recurse(d)


def options(opt):
    opt.add_option_group('configure options')
    copts = opt.get_option_group('configure options')
    copts.add_option('--tools-path',
                     default=None,
                     dest='flare_tools_path',
                     help='Path to tools (installed prefix)')
    copts.add_option('--compiler-prefix',
                     default=None,
                     dest='flare_compiler_prefix',
                     help='Compiler prefix')
    copts.add_option('--board',
                     default=None,
                     dest='flare_board',
                     help='Compiler prefix')
    copts.add_option('--xsa',
                     default=None,
                     dest='flare_xsa',
                     help='Path to XSA')
    copts.add_option('--ps-init',
                     default=None,
                     dest='flare_ps_init',
                     help='Path to PS initialisation file')


def configure(conf):
    tools_prefix = conf.options.flare_compiler_prefix
    if tools_prefix == None:
        conf.fatal('No compiler prefix found')

    board = conf.options.flare_board
    if board == None:
        conf.fatal('No board specified')
    else:
        conf.env.FLARE_BOARD = board

    tool_path_list = []
    if conf.options.flare_tools_path == None:
        tool_path_list = os.environ['PATH']
    else:
        if os.path.exists(os.path.join(conf.options.flare_tools_path, 'bin')):
            tool_path_list = os.path.join(conf.options.flare_tools_path, 'bin')
        else:
            tool_path_list = conf.options.flare_tools_path

    conf.find_program(tools_prefix + 'gcc',
                      path_list=tool_path_list,
                      var="CC")
    conf.find_program(tools_prefix + 'g++',
                      path_list=tool_path_list,
                      var="CXX")
    conf.find_program(tools_prefix + 'gcc',
                      path_list=tool_path_list,
                      var="LINK_CC")
    conf.find_program(tools_prefix + 'g++',
                      path_list=tool_path_list,
                      var="LINK_CXX")
    conf.find_program(tools_prefix + 'gcc',
                      path_list=tool_path_list,
                      var="AS")
    conf.find_program(tools_prefix + 'ld', path_list=tool_path_list, var="LD")
    conf.find_program(tools_prefix + 'ar', path_list=tool_path_list, var="AR")

    conf.load('gcc')
    conf.load('g++')
    conf.load('gas')

    conf.env.FLARE_TOP_DIR = str(conf.path.find_node('.'))

    if conf.options.flare_xsa and not conf.options.flare_ps_init:
        conf.env.FLARE_XSA = conf.options.flare_xsa
    elif not conf.options.flare_xsa and conf.options.flare_ps_init:
        conf.env.FLARE_PS_INIT = conf.options.flare_ps_init
    else:
        if board == 'zynqmp' or board == 'zynq7000':
            conf.fatal("xsa OR ps init is required for this board")
