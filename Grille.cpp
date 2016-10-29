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
array<GreedyList<uint16_t>,2500> Grille::coverNeighGraph;
array<GreedyList<uint16_t>,2500> Grille::connectNeighGraph;
bitset<2500> Grille::maskMatrix;
array<int,2500> Grille::distanceMatrix;

Grille::Grille(int t, int _rCapt, int _rCom):taille(t),rCapt(_rCapt),rCom(_rCom){
	nbCapteurs=0;
	if(!matrixInitialized){
		for(int i=0; i<2500; i++)
		{
			coverNeighGraph[i].resize(2500);
			connectNeighGraph[i].resize(2500);
		}
		for(int i=0; i<t; i++)
		for(int j=0; j<t; j++){
			if(i+j!=0)
			maskMatrix.set(taille*i+j);
			for(int dist=0; dist<=2*(taille-1);dist++)//je fais ça pour que les arretes qui appraissent en premier soient celle qui se rapproche de l'origine.
			for(int k=0; k<=dist; k++)
			{
				if(k>=taille || dist-k>=taille)
				continue;
				int l=dist-k;
				if((k-i)*(k-i)+(l-j)*(l-j)<=rCapt*rCapt){
					couvertMatrix[taille*i+j].set(t*k+l);
					if(k!=i || l!=j)
					coverNeighGraph[i*t+j].push_back(t*k+l);
				}
				if((k-i)*(k-i)+(l-j)*(l-j)<=rCom*rCom && (i!=k || j!=l)){
					connecteMatrix[t*i+j].set(t*k+l);
					connectNeighGraph[i*t+j].push_back(t*k+l);
					//connecteMatrix[i*t+j].set();
				}
				//distanceMatrix[i*t+j]=i+j;
			}
		}
		initializeDistanceMatrix(0);
		matrixInitialized=true;
	}
	for(int i=0; i<2500; i++)
	{
		coverGraph[i].resize(coverNeighGraph[i].size());
		connectGraph[i].resize(connectNeighGraph[i].size());
	}
	couvert.set(0);
	connecte.set(0);
}

void Grille::initializeDistanceMatrix(int index)
{
	int dist=distanceMatrix[index];
	if(index!=0)
	{
		distanceMatrix[index]=2500;
		for(auto i=connectNeighGraph[index].begin(); i!=connectNeighGraph[index].end() ;i++)
		if(distanceMatrix[*i]<distanceMatrix[index]-1)
		distanceMatrix[index]=distanceMatrix[*i]+1;
	}
	if(distanceMatrix[index]>dist || index==0)
	for(auto i=connectNeighGraph[index].begin(); i!=connectNeighGraph[index].end() ;i++)
	if(*i!=0)
	{
		initializeDistanceMatrix(*i);
	}

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
	std::fill(distanceMatrix.begin(), distanceMatrix.end(), 0);
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
			coverGraph[index].push_back(*k);
			coverGraph[*k].push_back(index);
		}
	}
	bool co=false;
	for(auto k=connectNeighGraph[index].begin(); k!=connectNeighGraph[index].end(); k++)
	{
		if(capteurs.test(*k)||*k==0){
			connectGraph[index].push_back(*k);
			connectGraph[*k].push_back(index);
			if(connecte.test(*k))
			co=true;
		}
	}
	if(co)
	connect(index);
}

void Grille::updateDistance(int index, int deletedCaptor)
{
	int dist=distance[index];
	distance[index]=2500;
	for(auto i=connectGraph[index].begin(); i!=connectGraph[index].end() ;i++)
	{
		if(distance[*i]<distance[index]-1)
		distance[index]=distance[*i]+1;
	}
	if(distance[index]>dist)
	for(auto i=connectGraph[index].begin(); i!=connectGraph[index].end() ;i++)
	if(distance[*i]>dist && *i!=deletedCaptor && *i!=0)
	updateDistance(*i,deletedCaptor);
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

bool Grille::eraseIfPossible(int index)
{
	if(!capteurs.test(index))
	exit(8);
	if(!checkCoverCaptor(index))
	{
		//cout<<"fast no cut"<<endl;
		return false;
	}
	if(inamovible.test(index))
	{
		//cout<<"fast no cut"<<endl;
		return false;
	}
	if(connectGraph[index].size()==1 || checkConnectCaptor(index))
	{
		//cout<<"fast cut"<<endl;
		nbCapteurs--;
		distance[index]=2500;
		capteurs.reset(index);
		for(auto k=coverGraph[index].begin(); k!=coverGraph[index].end();)
		{
			if(capteurs.test(*k))
			k++;
			else
			{
				coverGraph[*k].remove(index);
				k=coverGraph[index].erase(k);
			}
		}
		for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end();)
		{
			//si le capteur est un feuille, son voisin deviennent a priori amovibles.
			if(connectGraph[index].size()==1)
			inamovible.reset(*k);
			connectGraph[*k].remove(index);
			k=connectGraph[index].erase(k);
		}
		connecte.reset(index);
		return true;
	}
	else
	{
		marquage.reset();
		//marquage.set(index);
		bool flag;
		for(auto i=connectGraph[index].begin();i!=connectGraph[index].end(); i++)
		{
			if(*i==0)
			continue;
			//cout<<*i<<endl;
			if(distance[*i]<=distance[index])
			continue;
			if(marquage.test(*i))
			continue;
			marquage.reset();
			marquage.set(index);
			marquage.set(*i);
			flag=false;
			localConnect(*i,index,distance[index],flag);
			if(!flag) //on a pas pu connecter.
			{
				//cout<<"normal no cut"<<endl;
				return false;
			}
		}
		int dist=distance[index];
		distance[index]=2500;
		//cout<<"normal cut"<<endl;
		for(auto i=connectGraph[index].begin(); i!=connectGraph[index].end() ;i++)
		if(distance[*i]>dist)
		updateDistance(*i,index);
		nbCapteurs--;
		capteurs.reset(index);
		for(auto k=coverGraph[index].begin(); k!=coverGraph[index].end();)
		{
			if(capteurs.test(*k))
			k++;
			else
			{
				coverGraph[*k].remove(index);
				k=coverGraph[index].erase(k);
			}
		}
		for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end();)
		{
			connectGraph[*k].remove(index);
			k=connectGraph[index].erase(k);
		}
		connecte.reset(index);
		return true;
	}
}

bool Grille::localConnect(int index, int from, int dist, bool & flag) //analyse locale du graphe de connexion
{
	bool tree=true;
	if(connectGraph[index].size()==1)
	return true; //est sur une feuille
	for(auto i=connectGraph[index].begin(); i!=connectGraph[index].end(); i++)
	{
		if(marquage.test(*i)) //on rencontre un cycle
		{
			if(*i!=from)
			tree=false;
			continue;
		}
		marquage.set(*i);
		if(distance[*i]<=dist)//on reconnecte avec le puit
		{
			flag=true;
			//tree=false;
			return false;
		}
		else if(localConnect(*i,index,dist,flag)) //un des fils est un arbre (->sommet inamovible au risque de couper la branche)
		inamovible.set(index);
		else //un des fils n'est pas un arbre -> index n'est pas dans un arbre (il peut toujours être inamovible)
		tree=false;
	}
	return tree; //si index est inamovible
}

void Grille::connect(int index)
{
	connecte.set(index);
	for(auto k=connectGraph[index].begin(); k!=connectGraph[index].end(); k++)
	{
		if(!connecte.test(*k))
		connect(*k);
	}
}

void Grille::fill()
{
	nbCapteurs=taille*taille-1;
	capteurs=maskMatrix;
	connecte=maskMatrix;
	couvert=maskMatrix;
	couvert.set(0);
	connecte.set(0);
	inamovible.reset();
	coverGraph=coverNeighGraph;
	connectGraph=connectNeighGraph;
	distance=distanceMatrix;
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
	/*ret+="Capteurs inamovibles\n";
	for(int i=0; i<taille; i++){
		for(int j=0; j<taille; j++){
			ret+=inamovible.test(i*taille+j)?"1 ":"0 ";
		}
		ret+='\n';
	}
	cout<<"distance\n";
	for(int i=0; i<taille; i++){
	for(int j=0; j<taille; j++){
	cout<<distance[i*taille+j]<<" ";
	if(distance[i*taille+j]<10)
	cout<<"   ";
	else if(distance[i*taille+j]<100)
	cout<<"  ";
	else if(distance[i*taille+j]<1000)
	cout<<" ";
}
cout<<endl;
}*/
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
	bool self_cover=false;
	for(auto i=coverGraph[index].begin(); i!=coverGraph[index].end(); i++)
	{
		if(coverGraph[*i].size()==1 && !capteurs.test(*i) && *i!=0)
		return false;
		if(capteurs.test(*i))
		self_cover=true;
	}
	return self_cover;
}

bool Grille::checkConnectCaptor(int index) const
{
	bool ok=true;
	for(auto i=connectGraph[index].begin(); i!=connectGraph[index].end(); i++)
	{
		if(distance[index]>=distance[*i])
		continue;
		ok=false;
		for(auto j=connectGraph[*i].begin(); j!=connectGraph[*i].end(); j++)
		{
			if(*j!=index && distance[*j]<distance[*i])
			{
				ok=true;
				break;
			}
		}
		if(!ok)
		break;
	}
	return ok;
}

void Grille::rendRealisable(){
	vector<int> permutation(taille*taille-1);
	for(int i=0; i<taille*taille-1; i++)
	permutation[i]=i+1;
	random_shuffle(permutation.begin(), permutation.end());
	fill();
	for(auto i= permutation.begin(); i!=permutation.end(); i++)
	{
		/*eraseCaptor(*i);
		if(!estRealisable())
		addCaptor(*i);*/
		eraseIfPossible(*i);
		//cout<<toString()<<endl;
		//cin.get();
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
				else if(!aDroite && c2>newC){
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
			cout<<"unsuccessful : "<<cnt<<"/"<<best_score<<" : "<<g.getNbCapteurs()<<endl;
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
}

void Grille::voisinageLigneEtColonne(){
	Grille g2=Grille(taille, rCapt, rCom);
	g2=*this;
	Grille g3=Grille(taille, rCapt, rCom);
	g3=*this;
	for(int i=0; i<taille; i++){
		g2.flipColonneOuLigne(true, i);
		g2.combineHeur();
		if(g2.getNbCapteurs()<g3.getNbCapteurs())
		g3=g2;
		g2=*this;
		g2.flipColonneOuLigne(false, i);
		g2.combineHeur();
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
	eraseIfPossible(pivot);
	new_empty[0]=pivot;
	int cntcapt=0, cntempty=1;
	for(int i=0; empty[i]!=0; i++)
	{
		if(eraseIfPossible(empty[i]))
		{
			new_empty[cntempty]=empty[i];
			cntempty++;
		}
		else
		{
			new_capt[cntcapt]=empty[i];
			cntcapt++;
		}
	}
	for(int i=0; capt[i]!=0; i++)
	{
		if(capt[i]!=pivot)
		{
			if(eraseIfPossible(capt[i]))
			{
				new_empty[cntempty]=capt[i];
				cntempty++;
			}
			else
			{
				new_capt[cntcapt]=capt[i];
				cntcapt++;
			}
		}
	}
}

void Grille::recuitSimule()
{
	Grille gbest(*this);
	double rho=0.85;
	double T=1;
	int k=nbCapteurs*5;
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
	while(cnt<2 || T>0.1)
	{

		//clock_t t0=clock();
		for(int i=0;/*clock()-t0<CLOCKS_PER_SEC*0.1 ||*/ i<k; i++)
		{
			k=nbCapteurs*5;
			int pivot=rand()%nbCapteurs;
			random_shuffle(empty.begin(),empty.begin()+taille*taille-1-nbCapteurs);
			random_shuffle(capt.begin(),capt.begin()+nbCapteurs);
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
					cnt=0;
					best_score=last_score;
					cout<<"new best"<<best_score<<endl;
					gbest=g;
					cout<<gbest.structureValue()<<endl;
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

void Grille::voisinageDeuxLigneEtColonne(){
	Grille g2=Grille(taille, rCapt, rCom);
	g2=*this;
	Grille g3=Grille(taille, rCapt, rCom);
	g3=*this;
	for(int i=0;i<taille; i++){
		for(int j=i+1; j<taille; j++){
			g2.flipColonneOuLigne(true, i);
			g2.flipColonneOuLigne(true, j);
			g2.combineHeur();

			if(g2.getNbCapteurs()<g3.getNbCapteurs())
			g3=g2;
			g2=*this;
		}

		for(int j=0; j<taille; j++){

			g2.flipColonneOuLigne(true, i);
			g2.flipColonneOuLigne(false, j);
			g2.combineHeur();
			if(g2.getNbCapteurs()<g3.getNbCapteurs())
			g3=g2;
			g2=*this;
		}
	}
	for(int i=0; i<taille; i++){
		for(int j=i+1; j<taille; j++){

			g2.flipColonneOuLigne(false, i);
			g2.flipColonneOuLigne(false, j);
			g2.combineHeur();
			if(g2.getNbCapteurs()<g3.getNbCapteurs())
			g3=g2;
			g2=*this;
		}
	}
	*this=g3;


}

void Grille::voisDeuxLigneEtColonnePourVND(){
	Grille g2=Grille(taille, rCapt, rCom);
	g2=*this;

	for(int i=0;i<taille; i++){
		for(int j=i+1; j<taille; j++){
			g2.flipColonneOuLigne(true, i);
			g2.flipColonneOuLigne(true, j);
			g2.combineHeur();

			if(g2.getNbCapteurs()<nbCapteurs){
				*this=g2;
				return;
			}
			g2=*this;
		}

		for(int j=0; j<taille; j++){

			g2.flipColonneOuLigne(true, i);
			g2.flipColonneOuLigne(false, j);
			g2.combineHeur();
			if(g2.getNbCapteurs()<nbCapteurs){
				*this=g2;
				return ;
			}
			g2=*this;
		}
	}
	for(int i=0; i<taille; i++){
		for(int j=i+1; j<taille; j++){

			g2.flipColonneOuLigne(false, i);
			g2.flipColonneOuLigne(false, j);
			g2.combineHeur();
			if(g2.getNbCapteurs()<nbCapteurs){
				*this=g2;
				return;
			}
			g2=*this;
		}
	}


}

//Beaucoup trop onéreux en pratique...
void Grille::voisinageTroisLigneEtColonne(){
	Grille g2=Grille(taille, rCapt, rCom);
	g2=*this;
	Grille g3=Grille(taille, rCapt, rCom);
	g3=*this;
	for(int i=0;i<taille; i++){
		for(int j=i+1; j<taille; j++){
			for(int k=j+1; k<taille; k++){
				g2.flipColonneOuLigne(true, i);
				g2.flipColonneOuLigne(true, j);
				g3.flipColonneOuLigne(true, k);
				g2.combineHeur();
				if(g2.getNbCapteurs()<g3.getNbCapteurs())
				g3=g2;
				g2=*this;
			}
			for(int k=0; k<taille; k++){
				g2.flipColonneOuLigne(true, i);
				g2.flipColonneOuLigne(true, j);
				g3.flipColonneOuLigne(false, k);
				g2.combineHeur();
				if(g2.getNbCapteurs()<g3.getNbCapteurs())
				g3=g2;
				g2=*this;
			}
		}
		for(int j=0; j<taille; j++){
			for(int k=j+1; k<taille; k++){
				g2.flipColonneOuLigne(true, i);
				g2.flipColonneOuLigne(false, j);
				g3.flipColonneOuLigne(false, k);
				g2.combineHeur();
				if(g2.getNbCapteurs()<g3.getNbCapteurs())
				g3=g2;
				g2=*this;
			}
		}
	}
	for(int i=0;i<taille; i++){
		for(int j=i+1; j<taille; j++){
			for(int k=j+1; k<taille; k++){
				g2.flipColonneOuLigne(false, i);
				g2.flipColonneOuLigne(false, j);
				g3.flipColonneOuLigne(false, k);
				g2.combineHeur();
				if(g2.getNbCapteurs()<g3.getNbCapteurs())
				g3=g2;
				g2=*this;
			}
		}
	}

	*this=g3;

}

void Grille::VND(){

	int k=0;
	while(k<2){

		cout<<"k : "<<k<<endl;
		Grille g(taille, rCapt, rCom);
		g=*this;
		if(k==0)
		g.voisinageLigneEtColonne();
		else
		g.voisDeuxLigneEtColonnePourVND();

		if(g.getNbCapteurs()<nbCapteurs){
			*this=g;
			k=0;
		}
		else
		k++;
	}
}

double Grille::structureValue()
{
	array<int,50> cntfile;
	array<int,50> cntrow;
	std::fill(cntfile.begin(), cntfile.end(),0);
	std::fill(cntrow.begin(), cntrow.end(),0);
	double mean=(double)nbCapteurs/(double)taille;
	for(int i=0; i<taille; i++)
	{
		for(int j=0; j<taille; j++)
		{
			if(capteurs.test(taille*i+j))
			{
				cntrow[i]++;
				cntfile[j]++;
			}
		}
	}
	double value=0;
	for(int i=0; i<taille; i++)
	{
		value+=pow(cntfile[i],2);
		value+=pow(cntrow[i],2);
	}
	value/=(double)(2*taille);
	value-=(double)pow(mean,2);
	value=sqrt(value);
	value/=(double)mean;
	return value;
}
