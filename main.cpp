#include "Grille.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
	int t, rCapt, rCom;
	cin>>t>>rCapt>>rCom;
	fstream out;
    out.open("out.txt",fstream::trunc|fstream::out);
	Grille g(t, rCapt, rCom);
	g.rendRealisable();
	cout<<"Borne sup : "<<g.getNbCapteurs()<<endl;
	out<<g.toString();
	return 0;
}
