#!/bin/bash

install_path=$1
num=$2

tar -xvf usracc_install.tar
mv .t/* $install_path

for((i=0;i<$num;i=i+1))
do
	cp -a $install_path/usracc/cfg $install_path/usracc/cfg$i
	cp -a $install_path/usracc/data $install_path/usracc/data$i
	cp -a $install_path/usracc/log $install_path/usracc/log$i
done

rm -rf $install_path/usracc/cfg
rm -rf $install_path/usracc/data
rm -rf $install_path/usracc/log


echo "ulimit -n 99999" >> ~/.bash_profile
source ~/.bash_profile

echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$install_path/usracc/bin" >> ~/.bash_profile
source ~/.bash_profile

echo "net.core.somaxconn=2048" >> /etc/sysctl.conf
sysctl -p

echo "installed successfully"

