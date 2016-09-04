# -*- coding: utf-8 -*-
'''
Created on 2014-07-16
@summary: Trellis TEA use c and cython
@author: fiefdx
'''

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy

ctea = Extension(
    name = 'ctea',
    sources = ['lib/trellisctea.pyx', 'lib/ctealink.pxd', 'lib/ctea.c'],
    include_dirs = ['lib', numpy.get_include()]
)

setup(
    name = 'pyctea',
    version = '1.0.2',
    author = 'fiefdx',
    author_email = 'fiefdx@gmail.com',
    package_dir = {'pyctea':'src'},
    packages = ['pyctea'],
    cmdclass = {'build_ext': build_ext},
    ext_modules = [ctea]
)