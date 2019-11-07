#!/usr/bin/env zsh

source /etc/dowse/dir
export R=${DOWSE_DIR:-/opt/dowse}
source $R/dowse && dowse-stop
