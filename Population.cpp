#include "Population.hpp"
#include <iostream>
#include <map>
#include <cstdlib>
#include <iostream>
#include <fstream>



Population::Population(int t, int _rCapt, int _rCom, float temps, int tailleInitiale, float fractionGardee, int nbMaria, float nbAleatoire)
:taille(t),rCapt(_rCapt),rCom(_rCom), time(temps), nbMariage(nbMaria), support(Grille(t, rCapt, rCom)){
    
    meilleureValeur=taille*taille;

    for(int i=0; i<tailleInitiale; i++){
       // <<"i :"<<i<<endl;     
        support.poseCapteursAleatoires(t*t*nbAleatoire);
        support.combineHeur();
     //   support.descenteLocale();
     //   g.descenteLocaleLimitee(temps);
        cout<<support.getNbCapteurs()<<endl;
        valeurIndiv[i]=support.getNbCapteurs();
        popul[i]=support.getCapteurs();      
        fstream out;
        if(i==0){
            out.open("out0.txt",fstream::trunc|fstream::out);  
            out<<support.toString();
        }
        if(i==1){
            out.open("out1.txt",fstream::trunc|fstream::out);  
            out<<support.toString();
        }
        if(i==2){
            out.open("out2.txt",fstream::trunc|fstream::out);  
            out<<support.toString();
        }
        if(valeurIndiv[i]<meilleureValeur){
            meilleurCitoyen=popul[i];
            meilleureValeur=valeurIndiv[i];
        }
        support.videGrille();
    }
    int nbGarde=(int)(fractionGardee*tailleInitiale);
    nbrIndividu=tailleInitiale;
    selectionneIndividus(nbGarde);

    
}

void Population::marieDeuxIndividus(int p1, int p2, bitset<2500>& enfant1, bitset<2500>& enfant2){
    int l1=rand()%taille;
    int l2=rand()%taille;
    int c1=rand()%taille;
    int c2=rand()%taille;
    if(l1>l2){
        int temp=l1;
        l1=l2;
        l2=temp;
    }
    if(c1>c2){
        int temp=c1;
        c1=c2;
        c2=temp;
    }
    for(int l=0; l<taille; l++){
        for(int c=0; c<taille; c++){
            if(l1<=l && l<=l2 && c1<=c && c<=c2){
                enfant1[l*taille+c]=popul[p2][l*taille+c];
                enfant2[l*taille+c]=popul[p1][l*taille+c];
            }
            else{
                enfant1[l*taille+c]=popul[p1][l*taille+c];
                enfant2[l*taille+c]=popul[p2][l*taille+c];    
            }
        }
    }


}

void Population::selectionneIndividus(int nbrSelectionne){
    long total=0;
    map<long,int> proba;
    for(int i=0; i<nbrIndividu; i++){
        total+=(taille*taille/2-valeurIndiv[i])*(taille*taille/2-valeurIndiv[i])+1;
       // <<"total "<<total<<endl;
        proba.insert(pair<long,int> (total,i));
    }
    int count=0;
    set<int> gagnants;
    clock_t t1=clock();
    float diff=(float)(clock()-t1)/CLOCKS_PER_SEC;
    while (count<nbrSelectionne && diff<time){
        int alea=rand()%total;
       // <<"alea :"<<alea<<endl;
        map<long,int>::iterator it;
        pair<map<long,int>::iterator, bool> res;
        res=proba.insert(pair<long,int> (alea,0));
        it=res.first;
        
        if(res.second){
            it++;
            pair<set<int>::iterator, bool> resultat;
            resultat=gagnants.insert(it->second);
         //   <<"Insere :"<<it->second<<endl;
            if(resultat.second)
                count++;
            it--;
            proba.erase(it);
        }
        else{
            pair<set<int>::iterator, bool> resultat;
            resultat=gagnants.insert(it->second);
           // <<"Insere :"<<it->second<<endl;
            if(resultat.second)
                count++;
        }
        diff=(float)(clock()-t1)/CLOCKS_PER_SEC;
    }


    count=0;
    for(int i: gagnants){
        popul[count]=popul[i];
        valeurIndiv[count]=valeurIndiv[i];
        count++;
    }
    nbrIndividu=count;
}

void Population::selectionneElite(int nbrSelectionne){
    cout<<"Etape 0"<<endl;
    map<int,int> selection;
    cout<<"Etape 1 :"<<nbrIndividu<<endl;
    for(int i=0; i<nbrIndividu; i++)
        selection.insert(pair<int,int> (valeurIndiv[i],i));
    cout<<"Etape 2"<<endl;
    array<bitset<2500>,3002> temp1;
    array<int,3002> temp2;
    int count=0;
    for(auto it : selection){
        temp1[count]=popul[it.second];
        temp2[count]=valeurIndiv[it.second];
        count++;
        if(count==nbrSelectionne)
            break;
    }
    cout<<"Etape 3"<<endl;
    nbrIndividu=count;
    popul=temp1;
    valeurIndiv=temp2;
}

void Population::mute(bitset<2500>& indiv){
    for(int i=0; i<taille*taille; i++){
        long prob=rand()%1000;
        if(prob<=1){
       //     <<"On mute"<<endl;
            indiv.flip(i);
        }
    }
}

void Population::nouvelleGeneration(int nbMariage){
    for(int i=0; i<nbMariage; i++){
        cout<<"i :"<<i<<endl;
        bitset<2500> enfant1;
        bitset<2500> enfant2;
        int p1=rand()%(nbrIndividu-2*i); 
        int p2=rand()%(nbrIndividu-2*i);
        marieDeuxIndividus(p1, p2, enfant1, enfant2);
        mute(enfant1);
        mute(enfant2);
        support.fromBitset(enfant1);
        support.combineHeur();
        support.descenteLocale();
        //g.descenteLocaleLimitee(temps);
        enfant1=support.getCapteurs();
        support.videGrille();
        support.fromBitset(enfant2);
        support.combineHeur();
        support.descenteLocale();
        //g.descenteLocaleLimitee(temps);
        enfant2=support.getCapteurs();
        support.videGrille();


        popul[nbrIndividu]=enfant1;
        valeurIndiv[nbrIndividu]=enfant1.count();
        if(valeurIndiv[nbrIndividu]<meilleureValeur){
            meilleureValeur=valeurIndiv[nbrIndividu];
            meilleurCitoyen=popul[nbrIndividu];
        }
        nbrIndividu++;

        popul[nbrIndividu]=enfant2;
        valeurIndiv[nbrIndividu]=enfant2.count();
        if(valeurIndiv[nbrIndividu]<meilleureValeur){
            meilleureValeur=valeurIndiv[nbrIndividu];
            meilleurCitoyen=popul[nbrIndividu];
        }
        nbrIndividu++;
    }
    selectionneIndividus(nbrIndividu-2*nbMariage);
}

