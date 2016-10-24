#include "Grille.hpp"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include "GreedyList.hpp"
bool Grille::matrixInitialized=false;
array<bitset<2500>,2500> Grille::couvertMatrix;
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
					connectNeighGraph[i*t+j].push_back(t*k+l);
				}

			}
		}
		matrixInitialized=true;
	}
	couvert.set(0);
	connecte.set(0);
}

void Grille::reset()
{
	matrixInitialized=false;
	for(int i=0; i<2500; i++)
	{
		coverNeighGraph[i].clear();
		connectNeighGraph[i].clear();
		couvertMatrix[i].reset();
	}
	maskMatrix.reset();
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
	for(auto k=coverGraph[index].begin(); k!=coverGraph[index].end();)
	{
		if(capteurs.test(*k))
		{
			couvert.set(index);
			k++;
		}
		else
		{
			coverGraph[*k].remove(index);
			if(coverGraph[*k].empty())
			couvert.reset(*k);
			k=coverGraph[index].erase(k);
		}

	}
	for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end();)
	{
		connectGraph[*k].remove(index);
		k=connectGraph[index].erase(k);
	}
	connecte.reset();
	connect(0);

}

void Grille::connect(int index)
{
	connecte.set(index);
	for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end(); k++)
	if(!connecte.test(*k))
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
	/*ret+="Zone couverte\n";
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
	}*/
	ret+=estRealisable()?"La solution est réalisable.\n":"La solution n'est pas réalisable.\n";
	ret+=to_string(nbCapteurs);
	ret+=" capteurs sont utilisés.";
	return ret;
}
void Grille::fill()
{
	nbCapteurs=taille*taille-1;
	capteurs=maskMatrix;
	connecte=maskMatrix;
	couvert=maskMatrix;
	couvert.set(0);
	connecte.set(0);
	coverGraph=coverNeighGraph;
	connectGraph=coverNeighGraph;
}
void Grille::rendRealisable(){
	vector<int> permutation(taille*taille-1);
	for(int i=0; i<taille*taille-1; i++)
	permutation[i]=((i+1)*7)%(taille*taille);
	fill();
	for(auto i= permutation.begin(); i!=permutation.end(); i++)
	{
		eraseCaptor(*i);
		if(!estRealisable())
		addCaptor(*i);
	}
}
void Grille::neighImprove()
{
	int best_score=nbCapteurs;
	if(!estRealisable())
	return;

	list<int> permutation;
	GreedyList<list<int>::iterator> capt;
	GreedyList<list<int>::iterator> temp;
	capt.resize(taille*taille-1);
	temp.resize(taille*taille-1);
	for(int i=1; i<taille*taille; i++)
	if(!capteurs.test(i))
	{
		permutation.push_back(i);
	}
	for(int i=1; i<taille*taille; i++)
	if(capteurs.test(i))
	{
		permutation.push_back(i);
	}
	Grille g(taille, rCapt,rCom);
	int cnt;
	list<int>::iterator l;
	for(l=permutation.begin(),cnt=0; l!=permutation.end(); l++, cnt++)
	{
		if(cnt>=taille*taille-1-nbCapteurs)
		capt.push_back(l);
	}
	cnt=0;
	auto k=capt.begin();
	while(true)
	{

		int value=*(*k);
		*k=permutation.erase(*k);
		permutation.push_front(value);
		g.fill();
		for(auto i= permutation.begin(); i!=permutation.end(); i++)
		{
			g.eraseCaptor(*i);
			if(!g.estRealisable())
			{
				temp.push_back(i);
				g.addCaptor(*i);
			}
		}
		if(g.getNbCapteurs()<best_score)
		{
			capt=temp;
			best_score=g.getNbCapteurs();
			*this=g;
			k=capt.begin();
			cout<<"New best:"<<best_score<<endl;
			cnt=0;
			cout<<toString()<<endl;
			cin.get();
		}
		else
		{
			cnt++;
			//cout<<"unsuccessful : "<<cnt<<"/"<<best_score<<" : "<<g.getNbCapteurs()<<endl;
			if(cnt==best_score)
			{
				k=capt.end();
				l=permutation.end();
				return;
			}

			permutation.pop_front();
			permutation.insert(*k,value);
			k++;
		}
		temp.clear();
	}
}
