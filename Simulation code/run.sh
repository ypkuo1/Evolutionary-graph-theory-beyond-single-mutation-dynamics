# compiling the code
g++ -std=c++11 Crossing.cpp -o Crossing

# running the executable
# input: graph in edge list representation, output file, number of runs, mutation rate, selection on the intermediate mutatnt
./Crossing graphs/edgelist.txt results/out.txt 10000000 2e-6 -0.002
