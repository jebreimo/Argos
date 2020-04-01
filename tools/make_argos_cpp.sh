#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

python3 "$DIR"/mergecpp.py \
  "$DIR"/../include/Argos/*.[ch]pp \
  -o "$DIR"/../single_src/Argos.hpp

python3 "$DIR"/mergecpp.py \
  "$DIR"/../src/Argos/*.[ch]pp \
  --no-pragma-once \
  -p '#include "Argos.hpp"
' \
  -o "$DIR"/../single_src/Argos.cpp
