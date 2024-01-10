# Evolutionary-graph-theory-beyond-single-mutation-dynamics

Spatially-resolved datasets are revolutionizing knowledge in molecular biology, yet are under-utilized for questions in evolutionary biology. To gain insight from these large-scale datasets of spatial organization, we need mathematical representations and modeling techniques that can both capture their complexity, but also allow for mathematical tractability. Evolutionary graph theory utilizes the mathematical representation of networks as a proxy for heterogenous population structure and has started to reshape our understanding of how spatial structure can direct evolutionary dynamics. However, previous results are derived for the case of a single new mutation appearing in the population and the role of network structure in shaping fitness landscape crossing is still poorly understood. Here we study how network structured populations cross fitness landscapes and show that even a simple extension to a two-mutational landscape can exhibit complex evolutionary dynamics that cannot be predicted using previous single-mutation results. We show how our results can be intuitively understood through the lens of how the two main evolutionary properties of a network, the amplification and acceleration factors, change the expected fate of the intermediate mutant in the population and further discuss how to link these models to spatially-resolved datasets of cellular organization.

1. Compiling the code:
```bash
g++ -std=c++11 Crossing.cpp -o Crossing
```

2. Running the executable
```bash
#input: graph in edge list representation, output file, number of runs, mutation rate, selection on the intermediate mutatnt
./Crossing graphs/edgelist.txt results/out.txt 10000000 2e-6 -0.002
```
