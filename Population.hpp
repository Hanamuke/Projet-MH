#include "Grille.hpp"
#include <set>
using namespace std;

class Population{

private:
    array<bitset<2500>, 3002> popul;
    array<int, 3002> valeurIndiv;
    const int taille, rCapt, rCom;
    int nbrIndividu;
    bitset<2500> meilleurCitoyen;
    int meilleureValeur;
    const int time, nbMariage;
    Grille support;

public: 
    Population(int t, int _rCapt, int _rCom, float temps, int tailleInitiale, float fractionGardee, int nbMariage, float nbAleatoire);
    int getNbCapteurs(int i){return (popul[i]).count();}
    void marieDeuxIndividus(int p1, int p2, bitset<2500>& enfant1, bitset<2500>& enfant2);
    //sélectionne nbrSelectionne individu parmi les nbrIndividu premiers
    void selectionneIndividus(int nbrSelectionne);
    void selectionneElite(int nbrSelectionne);
    void mute(bitset<2500>& indiv);
    void nouvelleGeneration(int nbMariage);
    bitset<2500> getMeilleurCitoyen(){ return meilleurCitoyen;}
    int getMeilleureValeur(){return meilleureValeur;}

};