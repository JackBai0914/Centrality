g++ -std=c++11 combined.cpp -O3 -o combine -lpthread 
g++ -std=c++11 fst.cpp -O3 -o fst -lpthread -fsanitize=address -ftrapv -Xpreprocessor -fopenmp
g++ -std=c++11 gen.cpp -O3 -o gen
while true;do
	./gen >inp.in
 	time ./combine 4 >combine.out <inp.in
 	time ./fst >fst.out <inp.in

if diff combine.out fst.out
then
	echo 'AC'
else
	echo 'WA'
	exit
fi
done