mkdir -p tmp
cd tmp
wget https://github.com/google/benchmark/archive/v1.5.0.tar.gz
tar -xf v1.5.0.tar.gz
cd benchmark-1.5.0
mkdir build
cd build
cmake .. -GNinja
ninja
sudo ninja install