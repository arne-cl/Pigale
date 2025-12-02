"""
Tests for CLI converter tool.

Test-driven development for pigale-convert command-line tool.
"""

import pytest
import subprocess
import sys
from pathlib import Path
import tempfile

# Path to CLI script
CLI_SCRIPT = Path(__file__).parent.parent / "pigale_tgf" / "cli.py"


def test_cli_help():
    """Test that --help works."""
    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), "--help"],
        capture_output=True,
        text=True
    )
    assert result.returncode == 0
    assert "usage:" in result.stdout.lower()
    assert "pigale-convert" in result.stdout.lower() or "cli.py" in result.stdout.lower()


def test_cli_tgf_to_graphml(tmp_path):
    """Test converting TGF to GraphML."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "c.tgf"
    output_file = tmp_path / "output.graphml"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), str(input_file), str(output_file)],
        capture_output=True,
        text=True
    )

    assert result.returncode == 0, f"stderr: {result.stderr}"
    assert output_file.exists()
    assert output_file.stat().st_size > 0


def test_cli_txt_to_tgf(tmp_path):
    """Test converting TXT to TGF."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "a.txt"
    output_file = tmp_path / "output.tgf"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), str(input_file), str(output_file)],
        capture_output=True,
        text=True
    )

    assert result.returncode == 0, f"stderr: {result.stderr}"
    assert output_file.exists()
    assert output_file.stat().st_size > 0


def test_cli_graphml_to_txt(tmp_path):
    """Test converting GraphML to TXT."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "embed3d.graphml"
    output_file = tmp_path / "output.txt"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), str(input_file), str(output_file)],
        capture_output=True,
        text=True
    )

    assert result.returncode == 0, f"stderr: {result.stderr}"
    assert output_file.exists()
    assert output_file.stat().st_size > 0


def test_cli_auto_detect_output_format(tmp_path):
    """Test that output format is auto-detected from extension."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "c.tgf"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    # Test each output format
    for ext in [".graphml", ".txt", ".tgf"]:
        output_file = tmp_path / f"output{ext}"
        result = subprocess.run(
            [sys.executable, str(CLI_SCRIPT), str(input_file), str(output_file)],
            capture_output=True,
            text=True
        )
        assert result.returncode == 0, f"Failed for {ext}: {result.stderr}"
        assert output_file.exists()


def test_cli_missing_input_file():
    """Test error handling for missing input file."""
    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), "nonexistent.tgf", "output.txt"],
        capture_output=True,
        text=True
    )
    assert result.returncode != 0
    assert "not found" in result.stderr.lower() or "error" in result.stderr.lower()


def test_cli_invalid_output_format():
    """Test error handling for invalid output format."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "c.tgf"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), str(input_file), "output.invalid"],
        capture_output=True,
        text=True
    )
    assert result.returncode != 0
    assert "unsupported" in result.stderr.lower() or "invalid" in result.stderr.lower()


def test_cli_verbose_output(tmp_path):
    """Test verbose output mode."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "c.tgf"
    output_file = tmp_path / "output.txt"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), "-v", str(input_file), str(output_file)],
        capture_output=True,
        text=True
    )

    assert result.returncode == 0
    # Verbose mode should print something
    assert len(result.stdout) > 0 or len(result.stderr) > 0


def test_cli_record_selection(tmp_path):
    """Test TGF record selection."""
    tgf_dir = Path(__file__).parent.parent / "tgf"
    input_file = tgf_dir / "c.tgf"
    output_file = tmp_path / "output.txt"

    if not input_file.exists():
        pytest.skip(f"Test file not found: {input_file}")

    result = subprocess.run(
        [sys.executable, str(CLI_SCRIPT), "--record", "1", str(input_file), str(output_file)],
        capture_output=True,
        text=True
    )

    assert result.returncode == 0
    assert output_file.exists()
