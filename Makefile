all:
	g++ -o serial serial.cpp -lCppLinuxSerial

delete:
	rm serial