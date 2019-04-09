#!/bin/bash

find . -name '*.moos++'  -print -exec rm -rfv {} \;
find . -name 'MOOSLog*'  -print -exec rm -rfv {} \;
find . -name 'LOG_*'  -print -exec rm -rfv {} \;
find . -name 'targ_*'  -print -exec rm -rfv {} \;
find . -name '.LastOpenedMOOSLogDirectory'  -print -exec rm -rfv {} \;

echo "All logs and targ_ files in moos-ivp-simensov has been cleaned"
