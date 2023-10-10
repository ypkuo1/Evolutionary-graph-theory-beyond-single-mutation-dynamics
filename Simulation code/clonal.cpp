//
//  main.cpp
//  Clonal_interference
//
//  Created by Yang Ping Kuo on 9/9/22.
//

#include <iostream>
#include <iomanip>
#include "clonal.h"

static const int RUNTIME_LIMIT = 5;

int main(int argc, const char * argv[]) {
    Structure g(argv[1]);
    ofstream f_out;
    f_out.open(string(argv[2]) + ".txt");
    
    int trials = atoi(argv[3]);
    double mu = atof(argv[4]);
    int t_e = int(1 / mu);
    double s = atof(argv[5]);
    
    int landscape_width = atoi(argv[6]);
    
    Population p(g, s, mu);
        
    long start = time(NULL), runtime = 0;
    int trial = 0;
    
    
    f_out << "# mu = " << mu << ", s = " << s <<  ", trials requested = " << trials << ", landscape width = " << landscape_width << endl;
    f_out << "# \"Trial\"\t\"Steps\"\t\"Number of mutations\""<< endl;
    
    for(trial = 0; trial < trials; trial++)
    {
        int steps = 0;
        p.clear();
        
        int T = 0, t = 0;
        int t_next = p.get_next_mutation();
        int mut_cnt = 0;
        
        while (true)
        {
            while(t < t_e)
            {
                vector<int> bd_nodes = p.birth_death();
            
                if(t == t_next)
                {
                    mut_cnt = p.mutate(bd_nodes[0]);
                    t_next += p.get_next_mutation();
                    //cout << T << " "<< mut_cnt << endl;
                }
                else if(p.number_of_clones() == 1)
                {
                    t = t_next - 1;
                }
                t++;
                steps++;
                if (mut_cnt >= landscape_width)
                    break;
            }
            t -= t_e;
            t_next -= t_e;
            T++;
            if (mut_cnt >= landscape_width)
            {
                cout << trial << " " <<  T + 1 + static_cast<double>(t) / t_e << " "  << steps << " " << T << " " << p.number_of_mutations() << endl;
                f_out << trial << "\t" << T + 1 + static_cast<double>(t) / t_e << "\t" <<  p.number_of_mutations() << endl;
                break;
            }
            runtime = (time(NULL) - start);
            if (runtime / 3600 >=  RUNTIME_LIMIT)
            {
                f_out << "#" << T + 1 + static_cast<double>(t) / t_e << endl;
                break;
            }
        }
        if (runtime / 3600 >=  RUNTIME_LIMIT)
            break;
        
    }
    
    f_out << "# runtime = " << runtime / 3600 << ":";
    f_out << setfill ('0') << setw (2) << (runtime % 3600) / 60 << ":" << setfill ('0') << setw (2) << (runtime % 60) << endl;
    f_out.close();
    return 0;
}
