rm -rf cmake
mkdir -p cmake/build
cd cmake/build
cmake ../..
make
cp ../../node_list.txt .
cd ../..
