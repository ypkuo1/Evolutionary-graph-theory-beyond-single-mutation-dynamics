//  Evograph
//
//  Created by Yang Ping Kuo on 5/22/19.
//  Copyright © 2019 Yang Ping Kuo. All rights reserved.
//

#include <iostream>
#include "Single.h"
#include <random>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 5) {
        cout << "Not enough arguments!" << endl;
        exit(1);
    }
    
    string input = argv[1];
    Simulator sim(argv[1], argv[2]);
    
    int runs = atoi(argv[3]);
    cout << input << endl;
    cout << runs << endl;
    
    for(int i = 4; i < argc; ++i) {
        double s = atof(argv[i]);
        cout << s << endl;
        
        sim.simulate(runs, s);
        sim.print();
        sim.save();
    }
    
    for(int i = 4; i < argc; ++i) {
        double s = atof(argv[i]);
        cout << s << endl;
        
        sim.simulate_dB(runs, s);
        sim.print();
        sim.save();
    }
    
    
    return 1;
}
