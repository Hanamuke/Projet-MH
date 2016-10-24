#include "Grille.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
	list<pair<int,int>> r;
	list<int> t;
	r.push_back(pair<int,int>(1,1));
	r.push_back(pair<int,int>(1,2));
	r.push_back(pair<int,int>(2,2));
	r.push_back(pair<int,int>(2,3));
	r.push_back(pair<int,int>(3,3));
	r.push_back(pair<int,int>(3,4));
	t.push_back(10);
	t.push_back(15);
	t.push_back(20);
	t.push_back(25);
	t.push_back(30);
	t.push_back(40);
	t.push_back(50);
	srand(time(NULL));
	fstream out;
    out.open("out.txt",fstream::trunc|fstream::out);
	Grille g(10, 1, 1);
	/*for(auto i=r.begin(); i!=r.end(); i++)
	{
		for(auto j=t.begin(); j!=t.end(); j++)
		{
			Grille::reset();
			Grille g(*j, i->first, i->second);
			clock_t t0=clock();
			g.rendRealisable();
			g.neighImprove();
			clock_t t1=clock();
			out<<*j<<" "<< i->first<<" "<< i->second<<endl;
			out<<g.getNbCapteurs()<<endl;
			out<<(double)(t1-t0)/CLOCKS_PER_SEC<<endl<<endl;
			cout<<*j<<" "<< i->first<<" "<< i->second<<endl;
			cout<<g.getNbCapteurs()<<endl;
			cout<<(double)(t1-t0)/CLOCKS_PER_SEC<<endl<<endl;

		}
	}*/
	g.rendRealisable();
	cout<<"Borne sup : "<<g.getNbCapteurs()<<endl;
	cout<<g.toString()<<endl;
	cin.get();
	g.neighImprove();
	cout<<"Borne sup : "<<g.getNbCapteurs()<<endl;
	//out<<g.toString();
	return 0;
}
