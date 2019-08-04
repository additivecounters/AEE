# Creating the  executables
all: aee

aee:  ./*   
	g++ -Wall -o2 -march=native -fpermissive ./*.cc ./*.cpp -o AEE.exe -std=c++11

		
# Cleaning old files before new make
clean:
	rm -f AEE.exe 

