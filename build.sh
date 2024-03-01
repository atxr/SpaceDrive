echo "Building project and update sources in dist/"

cd src/mineziper/
mkdir build
cd build
cmake .. && make
cp bin/mineziperd ../../../dist
cd ..
rm -rf build
cd ../..

echo "Adding source backup to dist/"
zip -r dist/src.zip src