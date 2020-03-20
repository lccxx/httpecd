= HTTP epoll client demo

to build & test & run example
```bash
mkdir build
cd build
cmake ..  #  or cmake -DWITH_STATIC=ON -DWITH_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ..
make
make test
./httpecd
```
