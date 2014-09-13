#! /bin/sh

# Ejecutar este script para crear un ejecutable para cada nodo (un acceso directo al ejecutable).

BASEDIR=$(pwd)

echo $BASEDIR

echo Creando acceso para nodo 1
ln -sf $BASEDIR/src/asterix_bazar $BASEDIR/nodo1/asterix_bazar

echo Creando acceso para nodo 2
ln -sf $BASEDIR/src/asterix_bazar $BASEDIR/nodo2/asterix_bazar

echo Creando acceso para nodo 3
ln -sf $BASEDIR/src/asterix_bazar $BASEDIR/nodo3/asterix_bazar

echo Creando acceso para nodo 4
ln -sf $BASEDIR/src/asterix_bazar $BASEDIR/nodo4/asterix_bazar

