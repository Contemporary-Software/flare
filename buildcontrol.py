#
# Flare Build Control
#

def recurse(ctx, directories):
    for d in directories:
        ctx.recurse(d)

def options(opt):
    opt.add_option_group('configure options')
    copts = opt.get_option_group('configure options')
    copts.add_option('--tools-path',
                     default=None,
                     dest='flare_tools_path',
                     help='Path to tools')
    copts.add_option('--compiler_prefix',
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
        tool_path_list = conf.options.flare_tools_path

    conf.find_program(tools_prefix + '-gcc', path_list=tool_path_list, var="CC")
    conf.find_program(tools_prefix + '-g++', path_list=tool_path_list, var="CXX")
    conf.find_program(tools_prefix + '-gcc', path_list=tool_path_list, var="LINK_CC")
    conf.find_program(tools_prefix + '-g++', path_list=tool_path_list, var="LINK_CXX")
    conf.find_program(tools_prefix + '-gcc', path_list=tool_path_list, var="AS")
    conf.find_program(tools_prefix + '-ld', path_list=tool_path_list, var="LD")
    conf.find_program(tools_prefix + '-ar', path_list=tool_path_list, var="AR")

    conf.load('gcc')
    conf.load('g++')
    conf.load('gas')

    conf.env.FLARE_TOP_DIR = str(conf.path.find_node('.'))

    if conf.options.flare_xsa:
        conf.env.FLARE_XSA = conf.options.flare_xsa
    else:
        if board == 'zynqmp':
            conf.fatal("XSA is required for this board")
