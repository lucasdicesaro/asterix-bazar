#! /bin/sh

# Ejecutar este script solo por primera y unica vez, cuando se baja el proyecto de cero

echo Ejecutando aclocal
aclocal

echo Ejecutando autoconf
autoconf

echo Ejecutando touch AUTHORS NEWS README ChangeLog
touch AUTHORS NEWS README ChangeLog

echo Ejecutando automake --add-missing
automake --add-missing

echo Ejecutando ./configure
./configure

echo Ejecutando make
make
