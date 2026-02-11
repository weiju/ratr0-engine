import subprocess
import os

def build_docs():
    print("1. Running Doxygen...")
    # This assumes your Doxyfile is in the current directory
    subprocess.run(["doxygen", "Doxyfile"], check=True)

    print("2. Running Sphinx...")
    # -b html: build html
    # docs: source directory
    # docs/_build: output directory
    subprocess.run(["sphinx-build", "-b", "html", "docs", "docs/_build"], check=True)

    print("\nDone! Open docs/_build/index.html in your browser.")

if __name__ == "__main__":
    build_docs()