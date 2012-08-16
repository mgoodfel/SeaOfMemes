To build this under Ubuntu Linux 10.10, I installed the following packages:

sudo apt-get install build-essential
sudo apt-get install libgl1-mesa-dev

Build the mgUtil library:

cd ../../mgUtil/BuildLinux
make

Build the mg3D library:

cd ../../mg3D/BuildLinux
make

Then build this test program:

make


