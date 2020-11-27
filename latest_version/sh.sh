g++ gen.cpp -o gen -O2
./compile.sh
g++ ac.cpp -o ac -O2 -std=c++11

while true;do
	./gen >input.in
	time ./run.sh input.in
	time ./ac
if diff ac.out ans.txt
then
	echo 'AC'
else
	echo 'WA'
	exit
fi
done