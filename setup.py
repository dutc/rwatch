# rwatch - read watches

from setuptools import setup, find_packages, Extension
from os.path import join, dirname
from sys import version_info, exit

if version_info.major < 3 or version_info.minor < 5:
    exit("Sorry, Python 3.5 or above only")

rwatch = Extension('rwatch',
                   sources=['src/rwatch.c',
                            'src/hook.c',
                            'src/ceval.c',],
                   include_dirs=['src'], )

setup(
    name='dutc-rwatch',
    version='0.1.0',
    description='Read Watches',
    long_description=''.join(open(join(dirname(__file__),'README.md'))),
    author='James Powell',
    author_email='james@dontusethiscode.com',
    url='https://github.com/dutc/rwatch',
    packages=find_packages(exclude=['*demos*']),
    ext_modules=[rwatch],
)
