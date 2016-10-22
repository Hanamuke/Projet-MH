#include <vector>
#include <utility>
#include <bitset>

using namespace std;

class Grille{
private:
	int taille, rCapt, rCom;
	int nbCapteurs;
	//0 si la cible (i,j) n'est pas couvert par un capteur, 1 sinon
	bitset<2500> couvert;
	//0 s'il n'y a pas de capteurs en positon (i,j), 1 sinon
	bitset<2500> capteurs;
	//0 si le capteur n'est pas connecte au puit ou n'existe pas, 1 sinon
	bitset<2500> connecte; 



public:
	Grille(int t, int _rCapt, int _rCom);
	virtual int getNbCapteurs(){return nbCapteurs;}
	// getter qui renvoie la case (i,j) de couvert
	virtual bool estCouvert(int i, int j);
	// getter qui renvoie la case (i,j) de capteurs
	virtual bool contientCapteurs(int i, int j);
	//Transforme la disposition actuelle en une grille réalisable pour le problème
	virtual void rendRealisable();
	//Met à jour couvert quand on a ajouté un nouveau capteur
	virtual void miseAJourCouvert(int i, int j);
	//Met à jour l'accessibilité depuis le puit des capteurs, 
	//sachant les capteurs dans accessible viennent de devenir accessibles
	virtual void miseAjourCapteurs(vector<pair<int,int>> * accessible);
	//Met à jour l'accessibilité depuis le puit des capteurs,
	//sachant que le capteur (i,j) vient de devenir accessible
	virtual void miseAjourCapteursPart(int i, int j, vector<pair<int,int>> * accessible);
	//Retourne les coordonnées du plus proche capteur connecté au puit
	virtual pair<int,int> plusProcheConnecte(int i, int j);

};

