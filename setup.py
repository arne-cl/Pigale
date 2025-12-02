from setuptools import setup, find_packages

setup(
    name="pigale_tgf",
    version="0.1.0",
    description="Python implementation of Pigale TGF/TXT graph formats",
    author="Pigale TGF Port",
    packages=find_packages(),
    python_requires=">=3.9",
    install_requires=[
        "numpy>=1.20.0",
    ],
    extras_require={
        "test": [
            "pytest>=7.0.0",
            "pytest-cov>=3.0.0",
            "hypothesis>=6.0.0",
        ],
        "dev": [
            "black>=22.0.0",
            "flake8>=4.0.0",
            "mypy>=0.950",
        ],
    },
)
