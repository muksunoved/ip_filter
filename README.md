# IP filter project

## Download and install

### Add repository

~~~~
echo "deb https://dl.bintray.com/muksunoved/otus-packages xenial main" | sudo tee -a /etc/apt/sources.list
~~~~

###  Add repository sign key

~~~~
sudo apt-key adv --keyserver https://bintray.com/user/downloadSubjectPublicKey?username=muksunoved --recv-keys 7FBC7A6BDBF3D0EB
~~~~

### Install

~~~~
sudo apt update
sudo apt install ip_filter
~~~~

## Update new version

~~~~
sudo apt update
sudo apt upgrade
~~~~

## Remove

~~~~
sudo apt remove ip_filter
~~~~


