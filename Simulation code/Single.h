//  Evograph
//
//  Created by Yang Ping Kuo on 5/22/19.
//  Copyright © 2019 Yang Ping Kuo. All rights reserved.
//

#pragma once

#include <random>
#include <ctime>

#include <iostream>
#include <fstream>
#include <string>

#include <vector>
#include <stdexcept>

using namespace std;

void permute(int *iton, int *ntoi, int index1, int index2)
{
    int node1 = iton[index1];
    int node2 = iton[index2];
    
    iton[index1] = node2;
    iton[index2] = node1;
    
    ntoi[node1] = index2;
    ntoi[node2] = index1;
}

class Simulator {
private:
	int popsize;
	double s;
	int *degrees, **edgelist;
	int counts[2] = { 0, 0 };
	double times[2] = { 0, 0 };
	double rt;
    ofstream file;
	mt19937 generator;

public:
	Simulator(string, string);
    ~Simulator();
    void simulate(double);
    void simulate(int, double);
    void simulate_dB(double);
    void simulate_dB(int, double);
    void print();
    void save();
};

Simulator::Simulator(string input_name, string output_name) {
    ifstream input(input_name);
    file.open(output_name);
    
    vector<int> out, in;
    int node;
    int i = 0;
    popsize = 0;
    while (input >> node)
    {
        popsize = popsize < node ? node: popsize;
        if (i % 2 == 0)
            out.push_back(node);
        else
            in.push_back(node);
        ++i;
    }
    if (out.size() != in.size())
        throw invalid_argument("in and out should have same length");
    ++popsize;
    generator = mt19937((unsigned int)time(NULL));
    
    degrees = new int[popsize];
    int *temp = new int[popsize];
    
    for (int node = 0; node < popsize; ++node) {
        temp[node] = 0;
        degrees[node] = 0;
    }
    
    for (auto node : out)
        ++degrees[node];
    
    for (auto node : in)
        ++degrees[node];
    
    edgelist = new int*[popsize];
    for (int node = 0; node < popsize; ++node) {
        edgelist[node] = new int[degrees[node]];
    }
    
    for (int i = 0; i < in.size(); ++i) {
        int node1 = in[i];
        int node2 = out[i];
        edgelist[node1][temp[node1]] = node2;
        edgelist[node2][temp[node2]] = node1;
        ++temp[node1];
        ++temp[node2];
    }
    delete[] temp;
}

Simulator::~Simulator()
{
    for (int node = 0; node < popsize; ++node) {
        delete [] edgelist[node];
    }
    delete [] edgelist;
    delete [] degrees;
    file.close();
}


void Simulator::simulate(double s = 0)
{
    this->s = s;
    uniform_real_distribution<double> rand(0.0, 1.0);
    
    double fitness[] = { 1.f, 1.f + s };
    int populations[] = { popsize - 1, 1 };
    
    int *ntoi = new int[popsize];
    int *iton = new int[popsize];
    int *mutant = new int[popsize];
    
    for (int i = 0; i < popsize; ++i)
    {
        ntoi[i] = i;
        iton[i] = i;
        mutant[i] = 0;
    }
    
    //int index2 = popsize - 1;
    int index2 = (int)(rand(generator) * popsize);
    
    mutant[iton[index2]] = 1;
    permute(iton, ntoi, populations[0], index2);
    int t = 0;
    
    while (populations[0] != 0 && populations[1] != 0)
    {
        ++t;
        double bar = populations[0] * fitness[0];
        double totalFitness = bar + populations[1] * fitness[1];
        
        double birth = totalFitness * rand(generator);
        int birthIndex, birthNode, deathIndex, deathNode;
        
        if (birth > bar)
        {
            birthIndex = populations[0] + (int)((birth - bar) / fitness[1]);
            birthNode = iton[birthIndex];
        }
        else
        {
            birthIndex = (int)(birth / fitness[0]);
            birthNode = iton[birthIndex];
        }
        
        deathIndex = (int)(degrees[birthNode] * rand(generator));
        deathNode = edgelist[birthNode][deathIndex];
       
        if (mutant[deathNode] == mutant[birthNode])
            continue;
        
        if (mutant[deathNode] == 1)
        {
            --populations[1];
            index2 = populations[0];
            permute(iton, ntoi, ntoi[deathNode], index2);
        }
        else
        {
            --populations[0];
            index2 = (popsize - 1) - populations[1];
            permute(iton, ntoi, ntoi[deathNode], index2);
        }
        
        if (mutant[birthNode] == 1)
            ++populations[1];
        else
            ++populations[0];
        
        mutant[deathNode] = mutant[birthNode];
    }
    
    if (populations[0] == 0)
    {
        ++counts[1];
        times[1] += t;
    }
    else
    {
        ++counts[0];
        times[0] += t;
    }
    delete[] mutant;
    delete[] ntoi;
    delete[] iton;
}
// simulate birth-death processes for input trial number of times
void Simulator::simulate(int trials, double s = 0.0)
{
    generator = mt19937((unsigned int)time(NULL));
    clock_t start = clock();
    // Parallelized using openMP to create threads
    counts[0] = 0;
    counts[1] = 0;
    times[0] = 0;
    times[1] = 0;

    for (int i = 0; i < trials; ++i)
    {
        //cout << "run " << i << endl;
        simulate(s);
        if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC / 3600 > 1.0)
            break;
    }
    
    times[0] /= counts[0];
    times[1] /= counts[1];
    this->rt = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
    cout << rt << endl;
}

// simulates 1 death-birth process on network
void Simulator::simulate_dB(double s = 0)
{
    this->s = s;
    uniform_real_distribution<double> rand(0.0, 1.0);
    
    double fitness[] = { 1.f, 1.f + s };
    int populations[] = { popsize - 1, 1 };
    
    double *localFitness = new double[popsize];
    int *mutant = new int[popsize];
    
    for (int i = 0; i < popsize; ++i)
    {
        localFitness[i] = degrees[i];
        mutant[i] = 0;
    }
    
    int node, deathnode;
    int birthnode = (int)(rand(generator) * popsize);
    
    mutant[birthnode] = 1;
    
    for (int d = 0; d < degrees[birthnode]; ++d)
    {
        node = edgelist[birthnode][d];
        localFitness[node] += s;
    }
    
    int t = 0;
    while (populations[0] != 0 && populations[1] != 0)
    {
        ++t;
        deathnode = (int)(rand(generator) * popsize);
        double draw = rand(generator) * localFitness[deathnode];
        
        for (int d = 0; d < degrees[deathnode]; ++d)
        {
            node = edgelist[deathnode][d];
            draw -= (1 + s * mutant[node]);
            
            if (draw <= 0)
            {
                birthnode = node;
                break;
            }
            else if (d + 1 == degrees[deathnode])
                birthnode = node;
        }
        
        if (mutant[deathnode] == 1)
        {
            --populations[1];
            for (int d = 0; d < degrees[deathnode]; ++d)
            {
                node = edgelist[deathnode][d];
                localFitness[node] -= s;
            }
        }
        else
            --populations[0];
        
        if (mutant[birthnode] == 1)
        {
            ++populations[1];
            for (int d = 0; d < degrees[deathnode]; ++d)
            {
                node = edgelist[deathnode][d];
                localFitness[node] += s;
            }
        }
        else
            ++populations[0];
        mutant[deathnode] = mutant[birthnode];
    }
    
    if (populations[0] == 0)
    {
        ++counts[1];
        times[1] += t;
    }
    else
    {
        ++counts[0];
        times[0] += t;
    }
    delete[] mutant;
    delete[] localFitness;
}

void Simulator::simulate_dB(int trials, double s = 0.0)
{
    generator = mt19937((unsigned int)time(NULL));
    clock_t start = clock();
    // Parallelized using openMP to create threads
    counts[0] = 0;
    counts[1] = 0;
    times[0] = 0;
    times[1] = 0;
    
    for (int i = 0; i < trials; ++i)
    {
        //cout << "run " << i << endl;
        simulate_dB(s);
        if (static_cast<double>(clock() - start) / CLOCKS_PER_SEC / 3600 > 1.0)
            break;
    }
    
    times[0] /= counts[0];
    times[1] /= counts[1];
    this->rt = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
    cout << rt << endl;
}

void Simulator::print()
{
    printf("%f : %d,\t%d,\t%f,\t%f,\t%f\n", s, counts[0], counts[1], times[0], times[1], rt);
}

// save results of the simulations to output file stream
void Simulator::save()
{
    file << s << "\t";
    file << counts[0] << "\t";
    file << counts[1] << "\t";
    file << times[0] << "\t";
    file << times[1] << "\t";
    file << rt << endl;
}
