#include "Grille.hpp"
#include <set>
#include <mutex>
#include <thread>

using namespace std;

class Population{

private:
    array<bitset<2500>, 3002> popul;
    array<int, 3002> valeurIndiv;
    const int taille, rCapt, rCom;
    int nbrIndividu;
    //Bitset des capteurs du meilleur individu rencontre sur toutes les générations
    bitset<2500> meilleurCitoyen;
    int meilleureValeur;
    int moinsBonneValeur;
    vector<Grille> support;

public: 
    Population(int t, int _rCapt, int _rCom, int tailleInitiale, float fractionGardee);
    int getNbCapteurs(int i){return valeurIndiv[i];}
    Grille getCapteurs(int i);
    int getNbrIndividu(){return nbrIndividu;}
    //Effectue nbrExecution mariages en choisissant les parents parmi les finParent premiers individus de popul
    void marieDeuxIndividusArbre(int finParent, int numeroSupport, mutex & m, int nbrExecution);
    //trouve l'indice de la plus proche valeure supéreieur a alea dans le tableau trié tot
    int trouvePosition(double tot[], double alea);
    //sélectionne nbrSelectionne individu parmi les nbrIndividu premiers
    void selectionneIndividus(int nbrSelectionne);
    //Flip aléatoirement des bits de indiv
    void mute(bitset<2500>& indiv);
    // Construit une nouvelle generation qui aura la meme taille que l'ancienne en effectuant des mariages
    void nouvelleGeneration(int nbMariage);
    bitset<2500> getMeilleurCitoyen(){ return meilleurCitoyen;}
    int getMeilleureValeur(){return meilleureValeur;}
    //Effectue une descente locale sur tous les individus (non utilisée)
    void descenteDerniereGeneration();
    //Effectue une rotation d'angle pi/2 de centre (X1,Y1) des capteurs dans anciennePositions, puis place les capteurs avec pour centre X2,Y2
    bool pivote(int pointX1, int pointY1, int pointX2, int pointY2, vector<pair<int,int>>& anciennePosition, vector<pair<int,int>>& nouvel,YlePosition);
    //Genere des solutions aléatoires 
    void genereSolution(int nombre, int numeroSupport, mutex & m);
    //Renvoie un nombre aléatoire, de mainère thread safe
    static int intRand(const int & min, const int & max);
};