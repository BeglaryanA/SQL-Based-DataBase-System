MySQL: *.cpp *.h
	g++ -std=c++2a *.cpp Impl/*.cpp -o MySQL 
clean:
	rm -f MySQL