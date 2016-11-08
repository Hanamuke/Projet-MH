#if defined (_MSC_VER)  // Visual studio
    #define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
    #define thread_local __thread
#endif


#include "Population.hpp"
#include <iostream>
#include <map>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <functional>
#include <list>


Population::Population(int t, int _rCapt, int _rCom, int tailleInitiale, float fractionGardee)
:taille(t),rCapt(_rCapt),rCom(_rCom){
    
    for(int i=0; i<8; i++){
        Grille g(t, rCapt, rCom);
        support.push_back(g);
    }

    meilleureValeur=taille*taille;
    moinsBonneValeur=0;
    mutex m;
    nbrIndividu=0;
    int newTailleInit=(tailleInitiale/8)*8;
    list<thread> threads;
    for(int i=0; i<8; i++) //lancement des 8 theads.
        threads.emplace_back(&Population::genereSolution,this,newTailleInit/8,i,ref(m));
    for(auto i=threads.begin(); i!=threads.end(); i++)//attente de la fin des thread
        i->join();
    

    int nbGarde=(int)(fractionGardee*newTailleInit);
   // cout<<"nb Garde : "<<nbGarde;
    nbrIndividu=newTailleInit;
    selectionneIndividus(nbGarde);


    
}

void Population::genereSolution(int nombre, int numeroSupport, mutex & m){
    
   
    for(int i=0; i<nombre; i++){
        support[numeroSupport].poseCapteursAleatoires(50);
        support[numeroSupport].combineHeur();
        m.lock();
        valeurIndiv[nbrIndividu]=support[numeroSupport].getNbCapteurs();
        popul[nbrIndividu]=support[numeroSupport].getCapteurs();      
        if(valeurIndiv[nbrIndividu]<meilleureValeur){
            meilleurCitoyen=popul[nbrIndividu];
            meilleureValeur=valeurIndiv[nbrIndividu];
        }
        if(valeurIndiv[nbrIndividu]>moinsBonneValeur)
            moinsBonneValeur=valeurIndiv[nbrIndividu];
        support[numeroSupport].videGrille();
        nbrIndividu++;
        m.unlock();
    }
    
}


void Population::marieDeuxIndividusArbre(int finParent, int numeroSupport, mutex & m, int nbrExecution){
   
    for(int comptable=0; comptable<nbrExecution; comptable++){
        bitset<2500> enfant1;
        bitset<2500> enfant2;
        
        int p1=intRand(0,finParent-1); 
        int p2=intRand(0,finParent-1);


        bool fini=false;
        vector<pair<int,int>> v1; 
        vector<pair<int,int>> v2;
        vector<pair<int,int>> vNouvelle1;
        vector<pair<int,int>> vNouvelle2;
        while(!fini){
            v1.clear();
            v2.clear();
            int point1=intRand(0,taille*taille-1); 
            int point2=intRand(0,taille*taille-1);
            
        // cout<<"point 1 : "<<point1<<endl;
        // cout<<"point 2 : "<<point2<<endl;
            if(popul[p1].test((point1/taille) *taille+point1%taille) && popul[p2].test((point2/taille) *taille+point2%taille) ){
                v1.push_back(pair<int,int> (point1/taille, point1%taille));
                v2.push_back(pair<int,int> (point2/taille, point2%taille));
                support[numeroSupport].fromBitset(popul[p1]);
                set<int> ens;
            //   cout<<"Sous arbre 1 :"<<endl;
                support[numeroSupport].sousArbre(0, point1, v1, ens, false);
                ens.clear();
                support[numeroSupport].videGrille();
                support[numeroSupport].fromBitset(popul[p2]);
            //  cout<<"Sous arbre 2 :"<<endl;
                support[numeroSupport].sousArbre(0, point2, v2, ens, false);
                support[numeroSupport].videGrille();


                vector<pair<int,int>> vAncienne1;
                vector<pair<int,int>> vAncienne2;
                vNouvelle1=v1;
                vNouvelle2=v2;
                int k=0;
                bool fini1=false;
                while(k<4 && !fini1){
                    vAncienne1=vNouvelle1;
                    vNouvelle1.clear();
                    fini1=pivote(point1/taille, point1%taille, point2/taille, point2%taille, vAncienne1, vNouvelle1);
                    k++;
                }
                k=0;
                bool fini2=false;
                while(k<4 && !fini2){
                    vAncienne2=vNouvelle2;
                    vNouvelle2.clear();
                    fini2=pivote(point2/taille, point2%taille, point1/taille, point1%taille, vAncienne2, vNouvelle2);
                    k++;
                }
                fini=fini1&&fini2;  
            }
        }
    

        enfant1=popul[p1];
        enfant2=popul[p2];
        for(auto i : v1)
            enfant1.reset(i.first*taille+i.second);
        for(auto i : v2)
            enfant2.reset(i.first*taille+i.second);
        for(auto i : vNouvelle1)
            enfant1.set(i.first*taille+i.second);
        for(auto i : vNouvelle2)
            enfant2.set(i.first*taille+i.second);


        mute(enfant1);
        mute(enfant2);
        support[numeroSupport].fromBitset(enfant1);
        support[numeroSupport].combineHeur();
        enfant1=support[numeroSupport].getCapteurs();
        support[numeroSupport].videGrille();
        support[numeroSupport].fromBitset(enfant2);
        support[numeroSupport].combineHeur();
        enfant2=support[numeroSupport].getCapteurs();
        support[numeroSupport].videGrille();


        m.lock();
        popul[nbrIndividu]=enfant1;
        valeurIndiv[nbrIndividu]=enfant1.count();
        if(valeurIndiv[nbrIndividu]<meilleureValeur){
            meilleureValeur=valeurIndiv[nbrIndividu];
            meilleurCitoyen=popul[nbrIndividu];
        }
        if(valeurIndiv[nbrIndividu]>moinsBonneValeur)
            moinsBonneValeur=valeurIndiv[nbrIndividu];
        nbrIndividu++;

        popul[nbrIndividu]=enfant2;
        valeurIndiv[nbrIndividu]=enfant2.count();
        if(valeurIndiv[nbrIndividu]<meilleureValeur){
            meilleureValeur=valeurIndiv[nbrIndividu];
            meilleurCitoyen=popul[nbrIndividu];
        }
        if(valeurIndiv[nbrIndividu]>moinsBonneValeur)
            moinsBonneValeur=valeurIndiv[nbrIndividu];
        nbrIndividu++;
        m.unlock();
    }

}

int Population::trouvePosition(double tot[], double alea){
    int debut=0;
    int fin=nbrIndividu-1;
    int milieu=(debut+fin)/2;

    if(alea<tot[0])
        return 0;

    while(debut!=milieu){
      //  cout<<"tot milieu : "<<tot[milieu]<<", alea :"<<alea<<endl;
        if(alea<tot[milieu])
            fin=milieu;
        else
            debut=milieu;
        milieu=(debut+fin)/2;
    }

    return debut+1;
}

void Population::selectionneIndividus(int nbrSelectionne){
    double tot[nbrIndividu];
    double total=0;

    for(int i=0; i<nbrIndividu; i++){
        total+=exp(moinsBonneValeur-valeurIndiv[i]);
        if(total<0)
            cout<<"Et merde !"<<endl;
        tot[i]=total;
    }
    

    /*
    for(int i=0; i<nbrIndividu; i++){
        cout<<"tot[i]"<<tot[i]<<endl;
    }
    */
  //  bitset<3002> dejaSelectionne;
  //  dejaSelectionne.reset();
    set<int> gagnants;

    int count=0;
    while(count<nbrSelectionne){
        double dd=(double)rand()/(double)RAND_MAX;
        double alea=dd*total;
        int pos=trouvePosition(tot, alea);

        double poids=tot[0];
        if(pos!=0)
            poids=tot[pos]-tot[pos-1];
        for(int i=pos; i<nbrIndividu; i++)
            tot[i]-=poids;
        total-=poids;    
        gagnants.insert(pos);
        count++;
    }


    moinsBonneValeur=0;
    count=0;
    for(int it : gagnants){
        valeurIndiv[count]=valeurIndiv[it];
        popul[count]=popul[it];
        if(valeurIndiv[count]>moinsBonneValeur)
            moinsBonneValeur=valeurIndiv[count];
    //    cout<<"val choisi : "<<valeurIndiv[count]<<endl;
        count++;
    }

    nbrIndividu=count;
}


void Population::mute(bitset<2500>& indiv){
    for(int i=0; i<taille*taille; i++){
        int prob=rand()%100;
        if(prob<=1){
       //     <<"On mute"<<endl;
            indiv.flip(i);
        }
    }
}

void Population::nouvelleGeneration(int nbMariage){
 
    int newNbMariage=(nbMariage/8)*8;
    int finParent=nbrIndividu;
    mutex m;
    list<thread> threads;
    for(int i=0; i<8; i++) //lancement des 8 theads.
        threads.emplace_back(&Population::marieDeuxIndividusArbre,this,finParent,i,ref(m),newNbMariage/8);
    for(auto i=threads.begin(); i!=threads.end(); i++)//attente de la fin des thread
        i->join();

   // cout<<"nb Garde : "<<nbrIndividu-2*NewNbMariage;
    selectionneIndividus(nbrIndividu-2*newNbMariage);
}

void Population::descenteDerniereGeneration(){
    for(int i=0; i<nbrIndividu; i++){
        support[0].fromBitset(popul[i]);
        support[0].VND();
        popul[i]=support[0].getCapteurs();
        valeurIndiv[i]=support[0].getNbCapteurs();
        if(valeurIndiv[i]<meilleureValeur){
            meilleureValeur=valeurIndiv[i];
            meilleurCitoyen=popul[i];
        }
        support[0].videGrille();
    }
}

Grille Population::getCapteurs(int i){
    Grille g(taille, rCapt, rCom);
    g.fromBitset(popul[i]); 
    return g;   
}

bool Population::pivote(int pointX1, int pointY1, int pointX2, int pointY2, vector<pair<int,int>>& anciennePosition, vector<pair<int,int>>& nouvellePosition){
    bool faisable=true;
    for(auto i : anciennePosition){
        int dy=i.second-pointX1;
        int dx=i.first-pointX2;
        pair<int,int> p (pointX2-dy, pointY2+dx);
        nouvellePosition.push_back(p);
        if(p.first<0 || p.first>=taille || p.second<0 || p.second>=taille)
            faisable=false;
    }
    return faisable;
}


int Population::intRand(const int & min, const int & max) {
    static thread_local mt19937* generator = nullptr;  
    hash<thread::id> hasher;
    if (!generator) generator = new mt19937(clock() + hasher(this_thread::get_id()));
    uniform_int_distribution<int> distribution(min, max);
    return distribution(*generator);
}