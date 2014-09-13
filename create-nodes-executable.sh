#! /bin/sh

# Ejecutar este script para crear un ejecutable para cada nodo (un acceso directo al ejecutable).

echo Creando acceso para nodo 1
ln -s ./src/anjuta-bazar ./nodo1/anjuta-bazar

echo Creando acceso para nodo 2
ln -s ./src/anjuta-bazar ./nodo2/anjuta-bazar

echo Creando acceso para nodo 3
ln -s ./src/anjuta-bazar ./nodo3/anjuta-bazar

echo Creando acceso para nodo 4
ln -s ./src/anjuta-bazar ./nodo4/anjuta-bazar

