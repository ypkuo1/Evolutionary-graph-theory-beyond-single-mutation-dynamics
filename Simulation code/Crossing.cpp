//  Landscape crossing code
//
//  Created by Yang Ping Kuo on 5/22/20.
//  Copyright © 2020 Yang Ping Kuo. All rights reserved.
//

#include <iostream>
#include "Crossing.h"
#include <random>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{   
    if (argc < 6) {
        cout << "Not enough arguments!" << endl;
		exit(1);
    }
    
    string input = argv[1];
    Simulator sim(argv[1], argv[2]);
    
    int runs = atoi(argv[3]);
    double mu = atof(argv[4]);

    cout << input << endl;
    cout << runs << endl;
    cout << "mu=" << mu << endl;    

    for(int i = 5; i < argc; ++i) {
        double s = atof(argv[i]);
        //cout << s << endl;
        
        sim.simulate(runs, s, mu);
        sim.print();
        sim.save();
    }

    for(int i = 5; i < argc; ++i) {
        double s = atof(argv[i]);
        //cout << s << endl;
        
        sim.simulate_dB(runs, s, mu);
        sim.print();
        sim.save();
    }

    return 1;
}
