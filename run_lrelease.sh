#!/bin/bash

cd src; lrelease fightersfs.pro

mv *.qm gui/translations/

read -p "Press any key to continue... " -n1 -s
