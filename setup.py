from setuptools import setup, Extension
import sys
import setuptools
from codecs import open  # To use a consistent encoding
from os import path
from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext, ParallelCompile


here = path.abspath(path.dirname(__file__))

# Get the long description from the relevant file
with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()
    f.close()

with open("src/version.h") as f:
    line = f.read()
    __version__ = line.split('"')[1]
    f.close()

__package_name__ = 'socnet'


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


ext_modules = [
    Pybind11Extension(
        __package_name__,
        sorted(glob("src/*.cc")),
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True)
        ],
        language='c++'
    ),
]


ParallelCompile("NPY_NUM_BUILD_JOBS").install()

setup(
    name=__package_name__,
    version=__version__,
    author='Diego Carvalho',
    author_email='d.carvalho@ieee.org',
    url='https://github.com/diegomcarvalho/socnet',
    download_url='https://github.com/diegomcarvalho/socnet/archive/v' +
        str(__version__)+'.tar.gz',
    description='A socnet lib',
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.2'],
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    keywords=['socnet', 'covid', 'graph', 'net'],
    classifiers=[
        "License :: OSI Approved :: MIT License"
        ],
)
