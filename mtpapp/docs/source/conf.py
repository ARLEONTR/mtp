# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import sys
import os

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

sys.path.append( os.path.abspath('/home/kerneldev/mtp/linux-6.6.2/Documentation/sphinx'))
#sys.path.append(os.path.abspath('/home/kerneldev/mtp/linux-6.6.2/scripts'))

primary_domain = 'c'
extensions = ['sphinx.ext.autodoc', 'kerneldoc']
print(extensions)
source_suffix = '.rst'

project = 'MTP: Mesh Transport Protocol'
copyright = '2023, Ertan Onur'
author = 'Ertan Onur'
release = '0.01'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration


templates_path = ['_templates', '/home/kerneldev/mtp/linux-6.6.2/Documentation/sphinx/templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'bizstyle'
html_static_path = ['_static']
kerneldoc_bin = '/home/kerneldev/mtp/linux-6.6.2/scripts/kernel-doc'
kerneldoc_source = '/home/kerneldev/mtp/linux-6.6.2'
from load_config import loadConfig
loadConfig(globals())
