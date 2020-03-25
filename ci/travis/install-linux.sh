sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get update -qq
 
sudo apt-get install -qq g++-5
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 90
 
sudo apt install valgrind
sudo apt install cppcheck
sudo apt install gcovr
