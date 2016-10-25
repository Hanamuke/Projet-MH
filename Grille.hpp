#include <vector>
#include <utility>
#include <bitset>
#include <array>
#include <list>
#include <iostream>

using namespace std;

class Grille{
private:
	int nbCapteurs;
	const int taille, rCapt, rCom;
	static array<bitset<2500>,2500> couvertMatrix;
	static array<bitset<2500>,2500> connecteMatrix;
	static array<list<uint16_t>,2500> coverNeighGraph;
	static array<list<uint16_t>,2500> connectNeighGraph;
	static bitset<2500> maskMatrix;
	static bool matrixInitialized;
	//0 si la cible (i,j) n'est pas couvert par un capteur, 1 sinon
	bitset<2500> couvert;
	//0 s'il n'y a pas de capteurs en positon (i,j), 1 sinon
	bitset<2500> capteurs;
	//0 si le capteur n'est pas connecte au puit ou n'existe pas, 1 sinon
	bitset<2500> connecte;
	array<list<uint16_t>,2500> coverGraph;
	array<list<uint16_t>,2500> connectGraph;


public:
	Grille(int t, int _rCapt, int _rCom);
	//remplie la grille de capteurs.
	void fill();
	int getNbCapteurs(){return nbCapteurs;}
	// getter qui renvoie la case (i,j) de couvert
	bool estCouvert(int i, int j) const;
	bool estRealisable() const{return (couvert|(~maskMatrix)).all() && ((~connecte)&capteurs).none();}
	// getter qui renvoie la case (i,j) de capteurs
	bool contientCapteurs(int i, int j)const;
	//Transforme la disposition actuelle en une grille réalisable pour le problème
	void rendRealisable();
	//ajoute un capteur aux coordonnées souhaitées et conserve l'integrité des différents variables.
	void addCaptor(int i, int j);
	void addCaptor(int index);
	//supprime un capteur aux coordonnées souhaitées et conserve l'integrité des différents variables.
	void eraseCaptor(int i, int j);
	void eraseCaptor(int index);
	//effectue un parcours en profondeur à partir du sommet donnée pour connecter le graphe
	void connect(int index);
	//vérifie si un capteur est nécessaire à la couverture.
	bool checkCoverCaptor(int index) const;
	//Retourne les coordonnées du plus proche capteur connecté au puit
	pair<int,int> plusProcheConnecte(int i, int j);
	//Supprime des points aléatoirement pour en replacer d'autres
	void randomDelete(int n);
	//Renvoie une grille realisable à partir de la grille en cours
	void transRealisable();
	//donne le vecteur des sommets dans la meme composante connexe pour la connectivité
	void compConnexe(bitset<2500>& ciblesReliees, bitset<2500>& capteursReliees, int index);
	void augmenteDistance(bitset<2500>& bi);
	void ajouteCapteursPourRelier(int l1, int c1, int l2, int c2);
	bool verify();
	void ajouteCapteursPourCouvrir(int index);
	void combineHeur();
	void neighImprove();
	//Recherche une meilleure solution dans un voisinage ou on a flip une  ligne ou une colonne
	void voisinageLigneEtColonne();
	void voisinageDeuxLigneEtColonne();
	void voisinageTroisLigneEtColonne();
	//Recherche une meilleure solution dans un voisinage 2 flip et s'arrete à la premiere amélioration
	void voisDeuxLigneEtColonnePourVND();
	void VND();
	void flipColonneOuLigne(bool colonne, int n);
	

	Grille& operator=(Grille &arg) // copy/move constructor is called to construct arg
	{
		couvert=arg.couvert;
		capteurs=arg.capteurs;
		connecte=arg.connecte;
		coverGraph=arg.coverGraph;
		connectGraph=arg.connectGraph;
		nbCapteurs=arg.nbCapteurs;
	    return *this;
	}

	string toString() const;
	static void reset();


};
