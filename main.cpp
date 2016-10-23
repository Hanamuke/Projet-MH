#include "Grille.hpp"
#include <iostream>
#include <fstream>
#include <time.h>

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

	return 0;
}
