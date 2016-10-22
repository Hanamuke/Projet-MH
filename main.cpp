#include "Grille.hpp"
#include <iostream>

using namespace std;

int main(){

	int t, rCapt, rCom;
	cin>>t>>rCapt>>rCom;


	Grille * g=new Grille(t, rCapt, rCom);
	g->rendRealisable();
	cout<<"Borne sup : "<<g->getNbCapteurs()<<endl;

	return 0;

}


