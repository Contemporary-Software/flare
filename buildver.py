import os
import shutil
import time

outputs = {
    'default': ['bootloader/flare-build-id.c'],
    'versal': [],
    'zynqmp': [],
    'zynq7000': []
}

flare_build_ver_template = [
    '''/*
 * Copyright 2025 Contemporary Software
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

/*
 * Header file for generated file containing the unix time of build as an ID
 */

#include <flare-build-id.h>

uint32_t flare_build_id() {
    return ''', ''';
}'''
]


def build(bld):
    from waflib import Task

    def create_build_ver_c(task):
        output = str(task.outputs[0])
        with open(output, 'w') as bv:
            bv.write(flare_build_ver_template[0])
            bv.write(hex(int(time.time())))
            bv.write(flare_build_ver_template[1])

    class build_ver(Task.Task):
        color = 'CYAN'
        always_run = True
        run_str = [create_build_ver_c]

    board = bld.env.FLARE_BOARD
    outs = [bld.path.find_or_declare(file) for file in outputs['default']]
    outs += [bld.path.find_or_declare(file) for file in outputs[board]]

    build_ver_tsk = build_ver(env=bld.env)
    build_ver_tsk.set_outputs(outs)
    bld.add_to_group(build_ver_tsk)
