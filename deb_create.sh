#!/bin/bash

################################################################################

APP_NAME=fightersfs
APP_VER=0.1

################################################################################

APP_DIR=${APP_NAME}_${APP_VER}_focal_amd64

sudo rm -R $APP_DIR
sudo rm $APP_DIR.deb

mkdir $APP_DIR
mkdir $APP_DIR/DEBIAN
mkdir $APP_DIR/usr
mkdir $APP_DIR/usr/bin
mkdir $APP_DIR/usr/share
mkdir $APP_DIR/usr/share/applications
mkdir $APP_DIR/usr/share/icons
mkdir $APP_DIR/usr/share/icons/hicolor
mkdir $APP_DIR/usr/share/icons/hicolor/128x128
mkdir $APP_DIR/usr/share/icons/hicolor/128x128/apps

cp deb_fightersfs_amd64.control $APP_DIR/DEBIAN/control
cp bin/fightersfs $APP_DIR/usr/bin/
cp fightersfs.desktop $APP_DIR/usr/share/applications/
cp fightersfs.png $APP_DIR/usr/share/icons/hicolor/128x128/apps/

sudo chown root:root -R $APP_DIR
sudo chmod 0755 $APP_DIR/usr/bin/fightersfs
sudo chmod 0755 $APP_DIR/usr/share/applications/fightersfs.desktop
sudo chmod 0755 $APP_DIR/usr/share/icons/hicolor/128x128/apps/fightersfs.png

dpkg -b $APP_DIR/

################################################################################

DATA_DIR=${APP_NAME}-data_${APP_VER}_focal_all

sudo rm -R $DATA_DIR
sudo rm $DATA_DIR.deb

mkdir $DATA_DIR
mkdir $DATA_DIR/DEBIAN
mkdir $DATA_DIR/usr
mkdir $DATA_DIR/usr/share
mkdir $DATA_DIR/usr/share/fightersfs

cp deb_fightersfs-data_all.control $DATA_DIR/DEBIAN/control
cp -R data/ $DATA_DIR/usr/share/fightersfs

sudo chown root:root -R $DATA_DIR/

dpkg -b $DATA_DIR/

################################################################################

read -p "Press any key to continue..." -n1 -s
echo
