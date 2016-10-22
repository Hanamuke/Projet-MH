#include "Grille.hpp"
#include <algorithm>
#include <math.h>
#include <iostream>



Grille::Grille(int t, int _rCapt, int _rCom){
	taille=t;
	rCapt=_rCapt;
	rCom=_rCom;	
	nbCapteurs=0;
	
	
	for(int i=0; i<t; i++){
		for(int j=0; j<t; j++){
			couvert[i*t+j]=false;
			capteurs[i*t+j]=false;
			connecte[i*t+j]=false;
		}
	}
	couvert[0]=true;
	couvert[0]=true;
	connecte[0]=true;
}

bool Grille::estCouvert(int i, int j){
	return couvert[i*taille+j];
}

bool Grille::contientCapteurs(int i, int j){
	return capteurs[i*taille+j];
}

void Grille::miseAJourCouvert(int i, int j){
	for(int k=max(0,i-rCapt); k<min(taille,i+rCapt+1); k++){
		for(int l=max(0, j-rCapt); l<min(taille,j+rCapt+1); l++){
			if( sqrt((i-k)*(i-k)+(j-l)*(j-l)) <=rCapt)
				couvert[k*taille+l]=true;
		}
	}
}

void Grille::miseAjourCapteursPart(int i, int j, vector<pair<int,int>> * accessible){
	for(int k=max(0,i-rCom); k<min(taille,i+rCom+1); k++){
		for(int l=max(0, j-rCom); l<min(taille,j+rCom+1); l++){
			if( sqrt((i-k)*(i-k)+(j-l)*(j-l)) <=rCom && capteurs[k*taille+l] && !connecte[k*taille+l]){
				connecte[k*taille+l]=true;
				accessible->push_back(pair<int,int> (k,l));
			}
		}
	}
}

void Grille::miseAjourCapteurs(vector<pair<int,int>> * accessible){

	while( !accessible->empty()){
		int i=accessible->back().first;
		int j=accessible->back().second;
		accessible->pop_back();
		miseAjourCapteursPart(i,j, accessible);		
	}

}

pair<int,int> Grille::plusProcheConnecte(int i, int j){
	int distance=0;
	bool fini=false;
	int k1,l1;
	while(!fini){
		distance++;
		for(int k=max(0,i-distance); k<min(taille,i+distance+1); k++){
			for(int l=max(0, j-distance); l<min(taille,j+distance+1); l++){
				if( sqrt((i-k)*(i-k)+(j-l)*(j-l)) <=distance && connecte[k*taille+l]){
					k1=k;
					l1=l;
					fini=true;
				}
			}
		}
	}

	return pair<int,int> (k1,l1);
}

void Grille::rendRealisable(){

	cout<<"Initialisation ok"<<endl;

	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			if(!couvert[i*taille+j]){
				capteurs[i*taille+j]=true;
				nbCapteurs++;
				miseAJourCouvert(i,j);
			}
		}
	}


	cout<<"Etape 1 ok"<<endl;

	// Vecteur des capteurs accessibles depuis la source et 
	//dont on a pas encore regardé s'ils pouvaient mettre à jour d'autre capteurs
	vector<pair<int,int>> accessible;
	accessible.push_back(make_pair(0,0));

	miseAjourCapteurs(&accessible);


	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			if(!connecte[i*taille+j] && capteurs[i*taille+j]){
				pair<int,int> p=plusProcheConnecte(i, j);
				int k=p.first;
				int l=p.second;
				float dist=sqrt((i-k)*(i-k)+(j-l)*(j-l));
				int nbCaptManquants=(int)dist/(2*rCom);
				int d=1;
				//on rajoute des capteurs entre (i,j) et (k,l) afin que les deux puissent communiquer
				while(d<nbCaptManquants && !connecte[i*taille+j]){
					
					int n=(int)(k*d/nbCaptManquants+i*(nbCaptManquants-d)/nbCaptManquants);
					int m=(int)(l*d/nbCaptManquants+j*(nbCaptManquants-d)/nbCaptManquants);
					connecte[n*taille+m]=true;
					capteurs[n*taille+m]=true;
					nbCapteurs++;
					accessible.push_back(pair<int,int> (n,m));
					miseAjourCapteurs(&accessible);
					d++;
				}
			}
		}
	}

	cout<<"Etape 2 ok"<<endl;
}






