#include "Grille.hpp"
#include <algorithm>
#include <math.h>
#include <iostream>

bool Grille::matrixInitialized=false;
array<bitset<2500>,2500> Grille::couvertMatrix;
array<bitset<2500>,2500> Grille::connecteMatrix;
array<list<uint16_t>,2500> Grille::coverNeighGraph;
array<list<uint16_t>,2500> Grille::connectNeighGraph;
bitset<2500> Grille::maskMatrix;

Grille::Grille(int t, int _rCapt, int _rCom):taille(t),rCapt(_rCapt),rCom(_rCom){
	nbCapteurs=0;
	if(!matrixInitialized){
		for(int i=0; i<t; i++)
		for(int j=0; j<t; j++){
			if(i+j!=0)
			maskMatrix.set(taille*i+j);
			for(int k=0; k<t; k++)
			for(int l=0; l<t; l++){
				if((k-i)*(k-i)+(l-j)*(l-j)<=rCapt*rCapt){
					couvertMatrix[taille*i+j].set(t*k+l);
					if(k!=i || l!=j)
					coverNeighGraph[i*t+j].push_back(t*k+l);
				}
				if((k-i)*(k-i)+(l-j)*(l-j)<=rCom*rCom && (i!=k || j!=l)){
					connecteMatrix[t*i+j].set(t*k+l);
					connectNeighGraph[i*t+j].push_back(t*k+l);
				}

			}
		}
		matrixInitialized=true;
	}
	couvert.set(0);
	connecte.set(0);
}

bool Grille::estCouvert(int i, int j) const{
	return couvert[i*taille+j];
}

bool Grille::contientCapteurs(int i, int j) const{
	return capteurs[i*taille+j];
}

pair<int,int> Grille::plusProcheConnecte(int i, int j){
	int distance=0;
	bool fini=false;
	int k1=0,l1=0;
	while(!fini){
		distance++;
		for(int k=max(0,i-distance); k<min(taille,i+distance+1); k++){
			for(int l=max(0, j-distance); l<min(taille,j+distance+1); l++){
				if( (i-k)*(i-k)+(j-l)*(j-l)<=distance*distance && connecte[k*taille+l]){
					k1=k;
					l1=l;
					fini=true;
				}
			}
		}
	}

	return pair<int,int> (k1,l1);
}

void Grille::addCaptor(int i, int j)
{
	addCaptor(i*taille+j);
}
void Grille::addCaptor(int index)
{
	if(capteurs.test(index))
	return;
	nbCapteurs++;
	capteurs.set(index);
	couvert|=couvertMatrix[index];
	//j'utilise des pointeur qui s'entrepointent pour plus de vitesse mais je te conseille de pas trop y toucher
	for(auto k=coverNeighGraph[index].begin(); k!=coverNeighGraph[index].end(); k++)
	{
		if(!capteurs.test(*k)){
			coverGraph[index].push_front(*k);
			coverGraph[*k].push_front(index);
		}
	}
	bool co=false;
	for(auto k=connectNeighGraph[index].begin(); k!=connectNeighGraph[index].end(); k++)
	{
		if(capteurs.test(*k)||*k==0){
			connectGraph[index].push_front(*k);
			connectGraph[*k].push_front(index);
			if(connecte.test(*k))
			co=true;
		}
	}
	if(co)
	connect(index);
}

void Grille::eraseCaptor(int i, int j)
{
	eraseCaptor(taille*i+j);
}

void Grille::eraseCaptor(int index)
{
	if(!capteurs.test(index))
	return;
	nbCapteurs--;
	capteurs.reset(index);
	couvert.reset(index);
	for(auto k=coverGraph[index].begin(); k!=coverGraph[index].end(); k++)
	{
		if(capteurs.test(*k))
		couvert.set(index);
		else
		{
			coverGraph[*k].remove(index);
			if(coverGraph[*k].empty())
			couvert.reset(*k);
			coverGraph[index].remove(*k);
			k--;
		}
	}
	for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end(); k++)
	{
		connectGraph[*k].remove(index);
		connectGraph[index].remove(*k);
		k--;
	}
	connecte.reset();
	connect(0);
}

void Grille::connect(int index)
{
	if(connecte.test(index))
	return;
	connecte.set(index);
	for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end(); k++)
	connect(*k);

}

string Grille::toString() const
{
	string ret="Position des capteurs\n";
	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			ret+=capteurs[i*taille+j]?"1 ":"0 ";
		}
		ret+='\n';
	}
	ret+="Zone couverte\n";
	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			ret+=couvert[i*taille+j]?"1 ":"0 ";
		}
		ret+='\n';
	}
	ret+="Capteurs connectés\n";
	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			ret+=connecte[i*taille+j]?"1 ":"0 ";
		}
		ret+='\n';
	}
	ret+="Capteurs non connectés\n";
	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			ret+=capteurs[i*taille+j] && !connecte[i*taille+j]?"1 ":"0 ";
		}
		ret+='\n';
	}
	ret+=estRealisable()?"La solution est réalisable.\n":"La solution n'est pas réalisable.\n";
	ret+=to_string(nbCapteurs);
	ret+=" capteurs sont utilisés.";
	return ret;
}

void Grille::rendRealisable(){
for(int i=1; i<taille*taille; i++)
addCaptor(i);
for(int i=1; i<taille*taille; i++)
{
	eraseCaptor(i);
	if(!estRealisable())
	addCaptor(i);
}
/*eraseCaptor(1);
eraseCaptor(2);
cout<<toString()<<endl;*/
	/*cout<<"Initialisation ok"<<endl;

	for(int i=0; i<taille; i++)
	for(int j=0; j<taille; j++)
	if(!couvert[i*taille+j])
	addCaptor(i,j);
	cout<<"Etape 1 ok"<<endl;


	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			if(capteurs[i*taille+j] && !connecte[i*taille+j]){
				pair<int,int> p=plusProcheConnecte(i, j);
				int k=p.first;
				int l=p.second;
				float dist=sqrt((i-k)*(i-k)+(j-l)*(j-l));
				int nbCaptManquants=(int)dist/rCom;
				int d=1;
				//on rajoute des capteurs entre (i,j) et (k,l) afin que les deux puissent communiquer
				while(d<=nbCaptManquants && !connecte[i*taille+j]){

					int n=(int)(k*d/nbCaptManquants+i*(nbCaptManquants-d)/nbCaptManquants);
					int m=(int)(l*d/nbCaptManquants+j*(nbCaptManquants-d)/nbCaptManquants);
					addCaptor(n,m);
					d++;
				}
			}
		}
	}
	cout<<"Etape 2 ok"<<endl;
	eraseCaptor(6,3);
	eraseCaptor(7,6);*/
}
