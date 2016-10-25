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
					connecteMatrix[i*t+j].set();
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

void Grille::fill()
{
	nbCapteurs=taille*taille-1;
	capteurs=maskMatrix;
	connecte=maskMatrix;
	couvert=maskMatrix;
	couvert.set(0);
	connecte.set(0);
	coverGraph=coverNeighGraph;
	connectGraph=connectNeighGraph;
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

bool Grille::checkCoverCaptor(int index) const
{
	bool covered=false;
	for(auto i=coverGraph[index].begin(); i!=coverGraph[index].end(); i++)
	{
		if(coverGraph[*i].size()==1 && !capteurs.test(*i) && *i!=0)
		return true;
		if(capteurs.test(*i))
		covered=true;
	}

	return !covered;
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

bool Grille::verify()
{
	couvert.reset();
	for(int i=0; i<taille*taille; i++)
	if(capteurs.test(i))
	couvert|=couvertMatrix[i];
	if(!((couvert|~maskMatrix).all()))
	return false;
	connecte.reset();
	connect(0);
	if(((~connecte)&capteurs).any())
	return false;
	return true;
}

void Grille::randomDelete(int n){

	for(int compt=0; compt<n; compt++){
		int i=rand()%taille;
		int j=rand()%taille;
		while(!capteurs.test(i*taille+j)){
			i=rand()%taille;
			j=rand()%taille;
		}

		eraseCaptor(i*taille+j);
	}

	combineHeur();

}


void Grille::compConnexe(bitset<2500>& ciblesReliees, bitset<2500>& capteursReliees, int index){

	if(capteursReliees.test(index))
	return;
	capteursReliees.set(index);
	ciblesReliees|=connecteMatrix[index];
	for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end(); k++)
	compConnexe(ciblesReliees, capteursReliees, *k);
}

void Grille::augmenteDistance(bitset<2500>& bi){
	bitset<2500> bj=bi;
	bi.reset();
	for(int iter=0; iter<taille*taille; iter++){
		if(bj.test(iter)){
			bi.set(iter);
			int l=iter/taille;
			int c=iter%taille;
			if(l-1>=0)
			bi.set((l-1)*taille+c);
			if(l+1<taille)
			bi.set((l+1)*taille+c);
			if(c-1>=0)
			bi.set(l*taille+c-1);
			if(c+1<taille)
			bi.set(l*taille+c+1);
		}
	}
}

void Grille::ajouteCapteursPourRelier(int l1, int c1, int l2, int c2){
	if(l1<=l2){
		if(c1<=c2){
			int newL=l1;
			int newC=c1;
			bool aDroite=false;
			while((newL-l1)*(newL-l1)+(newC-c1)*(newC-c1)<=rCom){
				if(aDroite && l2>newL){
					newL++;
					aDroite=!aDroite;
				}
				else if(c2>newC){
					newC++;
					aDroite=!aDroite;
				}
				else if(l2>newL){
					newL++;
					aDroite=false;
				}
				else{
					newC++;
					aDroite=true;
				}
			}
			if(aDroite)
			newC--;
			else
			newL--;

			addCaptor(newL, newC);
			if((newL-l2)*(newL-l2)+(newC-c2)*(newC-c2)>rCom)
			return ajouteCapteursPourRelier(newL, newC, l2, c2);
			return;
		}
		else{
			int newL=l1;
			int newC=c1;
			bool aGauche=false;
			while((newL-l1)*(newL-l1)+(newC-c1)*(newC-c1)<=rCom){
				if(aGauche && l2>newL){
					newL++;
					aGauche=!aGauche;
				}
				else if(!aGauche && c2<newC){
					newC--;
					aGauche=!aGauche;
				}
				else if(l2>newL){
					newL++;
					aGauche=false;
				}
				else{
					newC--;
					aGauche=true;
				}
			}
			if(aGauche)
			newC++;
			else
			newL--;


			addCaptor(newL, newC);
			if((newL-l2)*(newL-l2)+(newC-c2)*(newC-c2)>rCom)
			return ajouteCapteursPourRelier(newL, newC, l2, c2);
			return;

		}
	}
	else
	return ajouteCapteursPourRelier(l2, c2, l1, c1);
}



void Grille::transRealisable(){
	for(int i=1; i<taille*taille; i++){
			if(!couvert[i]){
				if(rCapt==1 && rCom==1 && taille==10)
					addCaptor(i);
				else
					ajouteCapteursPourCouvrir(i);
			}
	}


	for(int i=0; i<taille*taille; i++){
			if(((~connecte)&capteurs)[i]){

				// bi est le bitset des capteurs qui sont dans la meme composante connexe
				bitset<2500> bi;
				bi.reset();
				bitset<2500> ciblesReliees;
				ciblesReliees.reset();
				compConnexe(ciblesReliees, bi, i);

				//bi devient le bitset des capteurs qui ne sont pas dans la composante connexe
				bi=capteurs&(~bi);
				int dist=rCom;
				while((bi&ciblesReliees).none()){
					dist++;
					augmenteDistance(ciblesReliees);
				}

				int indexAutreComp=0;
				while(!(bi&ciblesReliees).test(indexAutreComp))
					indexAutreComp++;
				int lAutreComp=indexAutreComp/taille;
				int cAutreComp=indexAutreComp%taille;
				int lCetteComp=max(0,lAutreComp-dist);
				int cCetteComp=max(0,cAutreComp-dist);
				bi=capteurs&(~bi);



				while(lCetteComp<=min(taille-1,lAutreComp+dist) && !(bi.test(lCetteComp*taille+cCetteComp))){
					while(cCetteComp<=min(taille-1,cAutreComp+dist) && !(bi.test(lCetteComp*taille+cCetteComp)))
						cCetteComp++;

					if( !(bi.test(lCetteComp*taille+cCetteComp)) ){
						cCetteComp=max(0,cAutreComp-dist);
						lCetteComp++;
					}
				}


				ajouteCapteursPourRelier(lCetteComp, cCetteComp, lAutreComp, cAutreComp);
				if(connecte.test(lAutreComp*taille+cAutreComp))
					connect(lCetteComp*taille+cCetteComp);



		}
	}

}

void Grille::combineHeur(){
	transRealisable();
	for(int i=1; i<taille*taille; i++)
	{
		if(capteurs.test(i)){
			eraseCaptor(i);
			if(!estRealisable())
				addCaptor(i);
		}
	}

}

void Grille::neighImprove()
{
	int best_score=nbCapteurs, cntcapt=0, cntempty=0;
	if(!estRealisable())
	return;
	vector<int> capt;
	capt.resize(taille*taille-1);
	vector<int> temp_capt;
	temp_capt.resize(taille*taille-1);
	vector<int> empty;
	empty.resize(taille*taille-1);
	vector<int> temp_empty;
	temp_empty.resize(taille*taille-1);
	for(int i=1; i<taille*taille; i++)
	if(!capteurs.test(i))
	{
		empty[cntempty]=i;
		cntempty++;
	}
	for(int i=1; i<taille*taille; i++)
	if(capteurs.test(i))
	{
		capt[cntcapt]=i;
		cntcapt++;
	}
	Grille g(taille, rCapt,rCom);
	int cnt=0;
	int k=capt[cnt];
	while(k!=0)
	{
		g.pivotDestructeur(capt,empty,temp_capt,temp_empty,k);
		if(g.getNbCapteurs()<best_score)
		{
			capt=temp_capt;
			empty=temp_empty;
			best_score=g.getNbCapteurs();
			*this=g;
			k=capt[0];
			cout<<"test "<<k<<endl;
			cout<<"New best:"<<best_score<<endl;
			cnt=0;
			//cout<<toString()<<endl;
			//cin.get();
		}
		else
		{
			cnt++;
			//cout<<"unsuccessful : "<<cnt<<"/"<<best_score<<" : "<<g.getNbCapteurs()<<endl;
			if(cnt==best_score)
			return;
			k=capt[cnt];
		}
	}
}


void Grille::ajouteCapteursPourCouvrir(int index){
	float distMax=0;
	int aPlacer=index;
	int l=index/taille;
	int c=index%taille;
	for(int i=max(0,l-rCapt); i<=min(taille-1,l+rCapt); i++){
		for(int j=max(0,c-rCapt); j<=min(taille-1,c+rCapt); j++){
			float dist=(l-i)*(l-i)+(c-j)*(c-j);
			if(!capteurs.test(i*taille+j) && dist<=rCom && dist>distMax){
				distMax=dist;
				aPlacer=i*taille+j;
			}
		}
	}

	addCaptor(aPlacer);
}

void Grille::flipColonneOuLigne(bool colonne, int n){
	if(colonne){
		for(int i=0; i<taille; i++){
			if(capteurs.test(i*taille+n))
				eraseCaptor(i*taille+n);
			else
				addCaptor(i*taille+n);
		}
	}
	else{
		for(int i=0; i<taille; i++){
			if(capteurs.test(n*taille+i))
				eraseCaptor(n*taille+i);
			else
				addCaptor(n*taille+i);
		}
	}
	combineHeur();
}

void Grille::voisinageLigneEtColonne(){
	Grille g2=Grille(taille, rCapt, rCom);
	g2=*this;
	Grille g3=Grille(taille, rCapt, rCom);
	g3=*this;
	for(int i=0; i<taille; i++){
			g2.flipColonneOuLigne(true, i);
			if(g2.getNbCapteurs()<g3.getNbCapteurs())
				g3=g2;
			g2=*this;
			g2.flipColonneOuLigne(false, i);
			if(g2.getNbCapteurs()<g3.getNbCapteurs())
				g3=g2;
			g2=*this;
	}

	*this=g3;
}

void Grille::pivotDestructeur(vector<int> const & capt, vector<int> const & empty, vector<int> & new_capt, vector<int> & new_empty, int pivot)
{
	fill();
	std::fill(new_capt.begin(), new_capt.end(), 0);
	std::fill(new_empty.begin(), new_empty.end(), 0);
	eraseCaptor(pivot);
	new_empty[0]=pivot;
	int cntcapt=0, cntempty=1;
	for(int i=0; empty[i]!=0; i++)
	{
			eraseCaptor(empty[i]);
			if(!estRealisable())
			{
				new_capt[cntcapt]=empty[i];
				cntcapt++;
				addCaptor(empty[i]);
			}
			else
			{
				new_empty[cntempty]=empty[i];
				cntempty++;
			}
	}
	for(int i=0; capt[i]!=0; i++)
	{
		if(capt[i]!=pivot)
		{
			eraseCaptor(capt[i]);
			if(!estRealisable())
			{
				new_capt[cntcapt]=capt[i];
				cntcapt++;
				addCaptor(capt[i]);
			}
			else
			{
				new_empty[cntempty]=capt[i];
				cntempty++;
			}
		}
	}
}

void Grille::recuitSimule()
{
	Grille gbest(taille, rCapt,rCom);
	double rho=0.85;
	double T=0.1*nbCapteurs/50;
	int k=nbCapteurs/0.2;
	int best_score=nbCapteurs;
	int last_score=best_score,cntcapt=0, cntempty=0, last_iter_score=best_score;
	if(!estRealisable())
	return;
	vector<int> capt;
	capt.resize(taille*taille-1);
	vector<int> temp_capt;
	temp_capt.resize(taille*taille-1);
	vector<int> empty;
	empty.resize(taille*taille-1);
	vector<int> temp_empty;
	temp_empty.resize(taille*taille-1);
	for(int i=1; i<taille*taille; i++)
	if(!capteurs.test(i))
	{
		empty[cntempty]=i;
		cntempty++;
	}
	for(int i=1; i<taille*taille; i++)
	if(capteurs.test(i))
	{
		capt[cntcapt]=i;
		cntcapt++;
	}
	Grille g(taille, rCapt,rCom);
	int cnt=0;
	while(cnt<5-taille/10)
	{
		k=nbCapteurs/0.2;
		for(int i=0; i<k; i++)
		{
			int pivot=rand()%nbCapteurs;
			random_shuffle(empty.begin(),empty.begin()+taille*taille-1-nbCapteurs);
			g.pivotDestructeur(capt,empty,temp_capt,temp_empty,capt[pivot]);
			double lambda=exp((double)(last_score-g.getNbCapteurs())/T);
			int p=100*lambda;

			if(rand()%100<p)
			{
				capt=temp_capt;
				empty=temp_empty;
				*this=g;
				last_score=g.getNbCapteurs();
				if(last_score<best_score)
				{
					best_score=last_score;
					cout<<"new best"<<best_score<<endl;
					gbest=g;
				}
			}
		}
		if(last_iter_score!=last_score)
		cnt=0;
		else
		cnt++;
		last_iter_score=last_score;
		T*=rho;
		cout<<"T="<<T<<endl;
		cout<<"current score"<<last_score<<endl;
	}
	*this=gbest;
}
