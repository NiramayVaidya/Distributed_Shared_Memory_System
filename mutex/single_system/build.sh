rm -rf cmake/build$1
mkdir -p cmake/build$1
cd cmake/build$1
cmake ../..
make
cp ../../node_list.txt .
cd ../..
