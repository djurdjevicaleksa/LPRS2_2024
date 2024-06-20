#!/bin/bash

exit 0

./waf configure

./waf build size upload --app=01 --port=/dev/ttyUSB0
