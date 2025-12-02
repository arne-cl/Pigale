"""Pytest configuration and fixtures."""

import pytest
from pathlib import Path


@pytest.fixture
def fixtures_dir():
    """Return path to test fixtures directory."""
    return Path(__file__).parent / "fixtures"


@pytest.fixture
def sample_tgf(fixtures_dir):
    """Return path to sample TGF file."""
    return fixtures_dir / "c.tgf"


@pytest.fixture
def sample_txt(fixtures_dir):
    """Return path to sample TXT file."""
    return fixtures_dir / "a.txt"
