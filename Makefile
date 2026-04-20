all:
	g++ -std=c++17 -Wall -Wextra -o train_nav main.cpp network.cpp

clean:
	del train_nav.exe test_ll.exe test_path.exe *.o

run: all
	./train_nav

test: all
	g++ -std=c++17 -Wall -Wextra -o test_ll tests/test_ll.cpp network.cpp
	g++ -std=c++17 -Wall -Wextra -o test_path tests/test_path.cpp network.cpp
	./test_ll
	./test_path
