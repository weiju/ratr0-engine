import os

# Get the absolute path to the current directory (where conf.py is)
current_dir = os.path.abspath(os.path.dirname(__file__))

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'ratr0-engine'
copyright = '2025-2026, Wei-ju Wu'
author = 'Wei-ju Wu'
release = '0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe', 'myst_parser']

templates_path = ['_templates']
exclude_patterns = []

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

# -- C Domain Configuration --------------------------------------------------
# Add any custom storage classes or macros that trip up the parser
c_id_attributes = ['__chip', '__interrupt', '__far']
c_aliases =  ['UINT32', 'UINT16', 'UINT8', 'INT32', 'INT16', 'INT8', 'BOOL']
c_paren_attributes = []

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_favicon = '_static/favicon.ico'
html_logo = "_static/logo.png"

# -- options for Breathe
breathe_default_project = "ratr0-engine"
breathe_projects = {
    "ratr0-engine": os.path.join(current_dir, "../../doxygen-out/xml")
}

# This allows you to use `struct MyStruct` instead of `struct my_namespace::MyStruct`
breathe_domain_by_extension = {
    "h" : "c",
    "c" : "c",
}

# -- UI Tweaks for RTD Theme -------------------------------------------------
html_theme_options = {
    'collapse_navigation': False,
    'sticky_navigation': True,
    'navigation_depth': 4,
    'titles_only': False
}

def setup(app):
    app.add_css_file('custom.css')