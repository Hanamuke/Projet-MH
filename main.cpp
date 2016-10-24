#include "Grille.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{

	int t, rCapt, rCom;
	//float facteurEnleve;
	cin>>t>>rCapt>>rCom;

	cout<<endl;
	clock_t t1, t2;



	fstream out2;
    out2.open("out2.txt",fstream::trunc|fstream::out);
	Grille g2(t, rCapt, rCom);
	t1=clock();
	g2.rendRealisable();
	t2=clock();
	cout<<"Borne sup heur 1: "<<g2.getNbCapteurs()<<endl;
	cout<<"Deuxieme temps : "<<(float)(t2-t1)/CLOCKS_PER_SEC<<endl;
	cout<<endl;
	out2<<g2.toString();


	fstream out5;
    out5.open("out5.txt",fstream::trunc|fstream::out);
	Grille g3(t, rCapt, rCom);
	t1=clock();
	g3.combineHeur();
	t2=clock();
	cout<<"Borne sup heur 3: "<<g3.getNbCapteurs()<<endl;
	cout<<"Troisieme temps : "<<(float)(t2-t1)/CLOCKS_PER_SEC<<endl;
	cout<<endl;
	out5<<g3.toString();

/*	fstream out3;
    out3.open("out3.txt",fstream::trunc|fstream::out);
	g2.randomDelete(g2.getNbCapteurs()*facteurEnleve);
	cout<<"Borne sup amelioree 1:"<<g2.getNbCapteurs()<<endl;
	out3<<g2.toString();


	fstream out6;
    out6.open("out6.txt",fstream::trunc|fstream::out);
	g3.randomDelete(g3.getNbCapteurs()*facteurEnleve);
	cout<<"Borne sup amelioree 3:"<<g3.getNbCapteurs()<<endl;
	out6<<g3.toString();

*/

	/*list<pair<int,int>> r;
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
	/*g.rendRealisable();
	cout<<"Borne sup : "<<g.getNbCapteurs()<<endl;
	cout<<g.toString()<<endl;
	cin.get();
	g.neighImprove();
	cout<<"Borne sup : "<<g.getNbCapteurs()<<endl;
	//out<<g.toString();*/
	return 0;
}
