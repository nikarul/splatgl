#!/bin/sh

aclocal
if [ $? -ne 0 ]; then
  echo "aclocal failed."
  echo "Failed to generate required build files, exiting."
  exit 1
fi

autoconf
if [ $? -ne 0 ]; then
  echo "autoconf failed."
  echo "Failed to generate required build files, exiting."
  exit 1
fi

automake -a
if [ $? -ne 0 ]; then
  echo "automake failed."
  echo "Failed to generate required build files, exiting."
  exit 1
fi

echo "You may now run ./configure"
