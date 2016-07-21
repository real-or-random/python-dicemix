# pylint: disable = bad-whitespace, missing-docstring

from setuptools import setup, find_packages

NAME = "dicemix"

setup(
    name = NAME,
    use_scm_version = True,
    packages = find_packages(exclude=['_solve.py']),
    include_package_data = True,
    setup_requires = ['cffi>=1.0.0', 'setuptools_scm'],
    cffi_modules = [NAME + '/solver/solver_build.py:ffibuilder'],
    install_requires = ['cffi>=1.0.0'],
    )
