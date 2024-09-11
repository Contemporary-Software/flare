import shutil

def unzip_xsa(path):
    shutil.unpack_archive(path, 'build/xsa', 'zip')
