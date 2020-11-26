# g++ -std=c++11 combined.cpp -O3 -o combine -lpthread

g++ ulimit -s unlimited
g++ brute.cpp -std=c++11 -O3 -o brute -lpthread -fsanitize=address -ftrapv