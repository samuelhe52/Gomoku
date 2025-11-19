#!/bin/bash

# Run from the script's directory to ensure relative paths work
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || exit 1

# Include LaTeX header to force first-line indentation in paragraphs
pandoc gomoku-report.md \
	-o gomoku-report.pdf \
	--pdf-engine xelatex \
	--pdf-engine-opt=-output-driver="xdvipdfmx -z 0"