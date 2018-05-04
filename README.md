# APT
Project repository for Advanced Programming Techniques, Spring 2018

This Project contains the simulation infrastructure used for testing the encryption based set indexing used to tackle cache side-channel attacks. The infrastructure can simulate multiple cores(tested for dual-core implementations)

To get started :
1) Clone the repository
2) cd APT/
3) mkdir build/
4) cd build
5) cmake ..
6) make

To run the simulator in single core mode with standard set indexing, use the following command

./sim -mode 3 -enc 0 ../traces/bzip2.mtr.gz

To enable AES based indexing, use the following command

./sim -mode 3 -enc 1 ../traces/bzip2.mtr.gz

To enable a dual core mode, set the following command

./sim -mode 4 -enc 0 ../traces/bzip2.mtr.gz ../traces/lbm.mtr.gz


TROUBLESHOOTING

This build was tested on gcc versions 5.4.0 (currently). If you encounter errors, we recommend you load a later version of gcc.
