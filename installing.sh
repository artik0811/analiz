#!bin/bash
echo "downloading analiz"
git clone https://github.com/artik0811/analiz
echo "downloading CppLinuxSerial"
git clone https://github.com/gbmhunter/CppLinuxSerial
echo "installing CppLinuxSerial"
cd CppLinuxSerial
mkdir build
cd build
cmake ~/CppLinuxSerial
sudo make install
 