#include "Grille.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <list>
#include <cmath>

using namespace std;

int main(int argc, char* argv[])
{
    fstream out;
    auto seed=time(NULL);
    srand(seed);
    out.open("out.txt",fstream::trunc|fstream::out);
    int taille,rcapt,rcomm;
    cin>>taille>>rcapt>>rcomm;
    Grille g(taille, rcapt,rcomm);
    g.rendRealisable();
    clock_t t0=clock();
    //décommenter la techniquer à utiliser
    //g.rendRealisable();
    //g.mtrecuit();
    if(!g.verify())
    exit(4);
    clock_t t1=clock();
    out<<taille<<" " << rcapt<<" "<< rcomm<<endl;
    out<<g.getNbCapteurs()<<endl;
    out<<(double)(t1-t0)/CLOCKS_PER_SEC<<endl<<endl;
    out<<g.toString()<<endl;
    cout<<taille<<" " << rcapt<<" "<< rcomm<<endl;
    cout<<g.getNbCapteurs()<<endl;
    cout<<(double)(t1-t0)/CLOCKS_PER_SEC<<endl<<endl;
    cout<<g.toString()<<endl;
    return 0;
}
