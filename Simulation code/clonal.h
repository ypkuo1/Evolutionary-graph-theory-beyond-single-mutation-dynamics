//
//  clonal.h
//  Clonal_interference
//
//  Created by Yang Ping Kuo on 9/9/22.
//

#pragma once
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include<cmath>

#include <random>
#include <vector>
#include <unordered_map>
#include<string>
#include <fstream>

using namespace std;

class Structure
{
public:
    int popsize;
    int *degrees, **edgelist;
    Structure(string);
    //~Structure();
};

Structure::Structure(string input_name)
{
    ifstream input(input_name);

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
        throw invalid_argument("In and out should have same length");
    ++popsize;
    
    degrees = new int[popsize];
    int *temp = new int[popsize];
    
    for (int node = 0; node < popsize; ++node)
    {
        temp[node] = 0;
        degrees[node] = 0;
    }
    
    for (auto node : out)
        ++degrees[node];
    
    for (auto node : in)
        ++degrees[node];
    
    edgelist = new int*[popsize];
    for (int node = 0; node < popsize; ++node)
        edgelist[node] = new int[degrees[node]];
    
    
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


class Clone
{
private:
    int clone_id;
    Clone* parent;
    double fitness;
    int count = 0;
    int children = 0;
    int mut_cnt = 0;
    
    vector<int> nodes;
    unordered_map<int, int> index {};
    
public:
    Clone(int clone_id, double fitness = 1) : clone_id(clone_id), fitness(fitness){ parent = nullptr; }
    
    Clone(int node, int clone_id, double fitness, Clone* parent_ = nullptr) : Clone(clone_id, fitness)
    {
        parent = parent_;
        if(parent != nullptr)
            parent->children++;
        
        nodes.push_back(node);
        mut_cnt = parent->get_mut_cnt() + 1;
        count++;
        index[node] = 0;
    }
    
    ~Clone()
    {
        if(parent != nullptr)
        {
            parent->children--;
            if(parent->children == 0 and parent->count == 0)
                delete parent;
        }
    }

    double total_fitness(){return fitness * count;}
    double get_fitness() {return fitness;}
    double get_mut_cnt() {return mut_cnt;}
    int get_count() {return count;}
    int sample_node(double p) {return nodes[(int)(p * count)]; }
    Clone* get_parent(){return parent;}
    int get_id(){return clone_id;}
    int get_children(){return children;}
    
    void add(int);
    void remove(int);
    friend ostream& operator<<(ostream& os, Clone& c)
    {
        for(int node:c.nodes)
            os << node << " ";
        
        os << endl;
        return os;
    }
};

void Clone::add(int node)
{
    if(index.count(node) )
        throw invalid_argument("Node alreadying in the clone!");
    
    nodes.push_back(node);
    index[node] = count;
    count++;
}

void Clone::remove(int node)
{
    if(index.count(node) == 0)
        throw invalid_argument("Node not in the clone!");
    
    count--;
    int idx = index[node];
    
    nodes[idx] = nodes[count];
    index[nodes[count]] = idx;
    
    nodes.pop_back();
    index.erase(node);
}


class Population
{
private:
    int N;
    double mu, s;
    
    Structure g;
    
    unordered_map<int, Clone*> cid_to_clone;
    int clone_id;
    
    Clone **node_to_clone;
    mt19937 generator;
    uniform_real_distribution<double> rand;
    geometric_distribution<int> rand_geo;
    
    void initialize()
    {
        clone_id = 0;
        
        cid_to_clone.clear();
        cid_to_clone[clone_id] = new Clone(clone_id, 1);
        
        node_to_clone = new Clone*[N];
        
        for(int node = 0; node < N; node++)
            cid_to_clone[clone_id]->add(node);
    
        for(int node = 0; node < N; node++)
            node_to_clone[node] = cid_to_clone[clone_id];
    }
    
    void tree_trace(Clone* clone, unordered_map<string, int> &edge_map)
    {
        if(clone->get_parent() != nullptr and edge_map.count(to_string(clone->get_id())) == 0)
        {
            Clone* parent = clone->get_parent();
            edge_map[to_string(clone->get_id())] = parent->get_id();
            tree_trace(parent, edge_map);
        }
    }
    
public:
    Population(Structure g, double s = 0, double mu = 0.001): g(g), s(s), mu(mu)
    {
        generator = mt19937((unsigned int)time(NULL));
        rand = uniform_real_distribution<double>(0.0,1.0);
        rand_geo = geometric_distribution<int> (mu);
        
        N = g.popsize;
        initialize();
    }
    
    void clear()
    {
        delete[] node_to_clone;
        for(auto cid : cid_to_clone)
            delete cid.second;
        initialize();
    }
    
    vector<int> birth_death()
    {
        vector<int> clone_id_vec{};
        vector<double> clone_weight {};
        
        for(auto cid : cid_to_clone)
        {
            clone_id_vec.push_back(cid.first);
            clone_weight.push_back(cid.second->total_fitness());
        }

        discrete_distribution<int> discrete(clone_weight.begin(), clone_weight.end());
        Clone* birth_clone = cid_to_clone[clone_id_vec[discrete(generator)]];
        int birth_node = birth_clone->sample_node(rand(generator) );
        
        int death_node = g.edgelist[birth_node][(int)(rand(generator) * g.degrees[birth_node]) ];
        Clone* death_clone = node_to_clone[death_node];
        
        if (death_clone != birth_clone)
        {
            birth_clone->add(death_node);
            death_clone->remove(death_node);

            node_to_clone[death_node] = node_to_clone[birth_node];
            
            if(death_clone->get_count() == 0)
            {
                cid_to_clone.erase(death_clone->get_id());
                if(death_clone->get_children() == 0)
                   delete death_clone;
            }
        }
        
        return vector<int>{birth_node, death_node};
    }
    
    int mutate(int parent_node)
    {
        Clone* parent_clone = node_to_clone[parent_node];
        clone_id ++;
        
        //cid_to_clone[clone_id] = new Clone(parent_node, clone_id, parent_clone->get_fitness() + s, parent_clone);
        //cout << parent_clone->get_mut_cnt() + 1 << endl;
        cid_to_clone[clone_id] = new Clone(parent_node, clone_id, parent_clone->get_fitness() * (1 + s), parent_clone);
        
        parent_clone->remove(parent_node);
        node_to_clone[parent_node] = cid_to_clone[clone_id];
        return cid_to_clone[clone_id]->get_mut_cnt();
        //return clone_id;
    }
    
    friend ostream& operator<<(ostream& os, Population& p)
    {
        for(auto cid : p.cid_to_clone)
            os << cid.first << ":" << cid.second->get_fitness() << endl;
        return os;
    }
    
    unordered_map<string, int> get_tree()
    {
        unordered_map<string, int> edge_map;
        for(auto cid : cid_to_clone)
        {
            edge_map[to_string(cid.second->get_id()) + "\'"] = cid.second->get_id();
            tree_trace(cid.second, edge_map);
        }
        return edge_map;
    }
    
    vector<double> get_node_fitnesses()
    {
        vector<double> node_to_fit(N, 0);
        for(int i = 0; i < N; i++)
            node_to_fit[i] = node_to_clone[i]->get_fitness();
        return node_to_fit;
    }
    
    int get_next_mutation(){return rand_geo(generator) + 1;} // Gotta add one since c++ geometric distribution starts at 0 instead of 1;
    int number_of_clones(){return static_cast<int>(cid_to_clone.size());}
    int number_of_mutations(){return clone_id;}
    
    unordered_map<int, int> mutation_distribution()
    {
        unordered_map<int, int> dist;
        for(auto cid : cid_to_clone)
        {
            int mut_cnt = cid.second->get_mut_cnt();
            int cnt = cid.second->get_count();
            if(dist.count(mut_cnt) )
                dist[mut_cnt] += cnt;
            else
                dist[mut_cnt] = cnt;
        }
        return dist;
    }
    
    vector<double> get_mut_cnt_moment()
    {
        vector<double> moment(2,0);
        for(auto cid : cid_to_clone)
        {
            moment[0] += cid.second->get_mut_cnt() * cid.second->get_count();
            moment[1] += pow(cid.second->get_mut_cnt(), 2) * cid.second->get_count();
        }
        moment[0] /= N;
        moment[1] /= N;
        moment[1] -=  pow(moment[0], 2);
        return moment;
    }
};
