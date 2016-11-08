#include <vector>
#include <utility>
#include <bitset>
#include <array>
#include <iostream>
#include "greedyList.hpp"
#include <ctime>
#include <thread>
#include <mutex>
#include <set>

using namespace std;

class Grille{
private:
	int nbCapteurs;
	const int taille, rCapt, rCom;
	//pour chaque sommet, bitset des sommets dans un voisinage rCapt
	static array<bitset<2500>,2500> couvertMatrix;
	//pour chaque sommet, matrice des sommets dans un voisinage rCom
	static array<bitset<2500>,2500> connecteMatrix;
	//graphe Gcapt : pour chaque sommet, liste des sommets dans un voisinage rCapt
	static array<GreedyList<uint16_t>,2500> coverNeighGraph;
	//graphe Gcomm : pour chaque sommet, liste des sommets dans un voisinage rCom
	static array<GreedyList<uint16_t>,2500> connectNeighGraph;
	//bitset des sommets que l'on veut capter, utile lorsque taille<50 car taille^2<2500
	static bitset<2500> maskMatrix;
	//distance des capteurs au puits lorsque la grille est remplie de capteurs, après un fill
	static array<int,2500> distanceMatrix;
	//booléen qui indique si les variables statiques on été initialisées.
	static bool matrixInitialized;
	//0 si la cible (i,j) n'est pas couvert par un capteur, 1 sinon
	bitset<2500> couvert;
	//0 s'il n'y a pas de capteurs en positon (i,j), 1 sinon
	bitset<2500> capteurs;
	//0 si le capteur n'est pas connecte au puit ou n'existe pas, 1 sinon
	bitset<2500> connecte;
	//all-purpose bitset for marking captors
	bitset<2500> marquage;
	//graphe Gcapt(S) : pour chaque sommet sans capteur, liste des capteurs qui captent le sommet. Pour chaque sommet avec capteur, équivalent à coverNeighGraph
	array<GreedyList<uint16_t>,2500> coverGraph;
	//graphe Gcomm(S) : pour chaque sommet avec capteur, liste des capteurs communiquant avec lui (puits inclu)
	array<GreedyList<uint16_t>,2500> connectGraph;
	//distance des capteurs à l'origine
	//devient invalide sur utilisation de eraseCaptor et addCaptor, à utiliser uniquement avec fill et eraseIfPossible
	array<int,2500> distance;
	//bit à 1 si le sommet à été démontré inamovible par localConnect, un bit 0 ne garanti pas que le sommet soit amovible.
	//devient invalide sur utilisation de eraseCaptor et addCaptor, à utiliser uniquement avec fill et eraseIfPossible
	bitset<2500> inamovible;

public:
	Grille(int t, int _rCapt, int _rCom);
	//Construit la grille pour que les données soient intègres, à partir du bitset des capteurs capt
	void fromBitset(bitset<2500>& capt);
	//remplie la grille de capteurs.
	void fill();
	// à appeler sur 0
	void initializeDistanceMatrix(int index);
	int getNbCapteurs(){return nbCapteurs;}
	// getter qui renvoie la case (i,j) de couvert
	bool estCouvert(int i, int j) const;
	//renvoire true si la solution est réalisable
	bool estRealisable() const{return (couvert|(~maskMatrix)).all() && ((~connecte)&capteurs).none();}
	// getter qui renvoie la case (i,j) de capteurs
	bool contientCapteurs(int i, int j)const;
	//Transforme la disposition actuelle en une grille réalisable pour le problème
	void rendRealisable();
	//ajoute un capteur aux coordonnées souhaitées et met à jour les graphes
	void addCaptor(int i, int j);
	void addCaptor(int index);
	bitset<2500> getCapteurs(){return capteurs;}
	//supprime un capteur aux coordonnées souhaitées et met à jour les graphes
	void eraseCaptor(int i, int j);
	void eraseCaptor(int index);
	//effectue un parcours en profondeur à partir du sommet donnée pour connecter le graphe
	void connect(int index);
	//vérifie si un capteur est nécessaire à la couverture.
	bool checkCoverCaptor(int index) const;
	//si la fonction renvoie vrai, le capteur est nécessaire. NE PAS UTILISER EN DEHORS DE eraseIfPorssible (utilise distance et inamovible)
	bool checkConnectCaptor(int index) const;
	//Retourne les coordonnées du plus proche capteur connecté au puit
	pair<int,int> plusProcheConnecte(int i, int j);
	//Supprime des points aléatoirement pour en replacer d'autres
	void randomDelete(int n);
	// Enlève tous les capteurs de la grille en maintenant l'intégrité des attributs
	void videGrille();
	//Renvoie une grille realisable à partir de la grille en cours
	void transRealisable();
	//donne le vecteur des sommets dans la meme composante connexe pour la connectivité
	void compConnexe(bitset<2500>& ciblesReliees, bitset<2500>& capteursReliees, int index);
	//Sert à augmenter la distance a laquelle on recherche un voisin de la composante connexe
	void augmenteDistance(bitset<2500>& bi);
	//Etant donne deux positions de capteurs, place un nombre minimum de capteurs pour relier ces deux capteurs pour la connexion
	void ajouteCapteursPourRelier(int l1, int c1, int l2, int c2);
	//renvoie true si la solution est réalisable, en utilisant uniquement capteurs.
	bool verify();
	//Relie la composante à laquelle le capteur en position i appartient au puits
	void relieComposanteAuPuit(int i);
	// ajoute un capteur le plus éloigné possible de index tout en assurant que index soit couvert
	void ajouteCapteursPourCouvrir(int index);
	//Applique la méthode constructive pour toruver une grille réalisable, puis elimine le surplus de capteurs
	void combineHeur();
	// recherche de voisinages ameiliorant pour l'heuristique destructrice
	//void neighImprove();
	//Recherche une meilleure solution dans un voisinage ou on a flip une  ligne ou une colonne
	void voisinageLigneEtColonne();
	//Recherche une meilleure solution dans un voisinage ou on a flip trois ligne(s) ou colonne(s)
	//void voisinageTroisLigneEtColonne();
	//Recherche une meilleure solution dans un voisinage 2 flip et s'arrete à la premiere amélioration
	void voisDeuxLigneEtColonnePourVND();
	// Applique une VND pour les voisinnages 1 et 2 flip (rajouter 3-flip n'améliore pas)
	void VND();
	//Flip de manière intègre tous les bits de la ligne n si bool vaut false, de la colonne n sinon
	void flipColonneOuLigne(bool colonne, int n);
	//methode de base de l'heuristique destructrice
	void pivotDestructeur(vector<int> const & capt, vector<int> const & empty, vector<int> & new_capt, vector<int> & new_empty, int pivot);
	//met à jour la distance de index , voisin du point retiré;
	void updateDistance(int index, int deletedCaptor);
	//fait ce que son nom suggère, cette fonction utilise des optimisations complexes à l'aide de distance, absolument ne pas utiliser
	//conjointement avec eraseCaptor et addCaptor
	bool eraseIfPossible(int index);
	//utiliser uniquement dans eraseIfPossible. regarde si le sommet index est connecté au puit à l'aide d'une analyse locale u graphe et de distance
	//distanceUpdate contient alors la liste des capteur donc la distance doit être mise à jour si on enlève index. c'est à dire si les autres voisin
	//de index ne posent pas de problème non plus. L'analyse local permet également l'obtention d'information sur les sommets inamovibles.
	//Retourne true si les composantes connexes voisines de index (autres que celle de from), sont des arbres.
	bool localConnect(int index, int from, int distance, bool & flag);
	//Pose de manière intègre et aléatoire n capteurs 
	void poseCapteursAleatoires(int n);
	//Donne tous les points dont le chemin pour allé au puits passe par pointCons
	void sousArbre(int i, int pointCons, vector<pair<int,int>>& elements, set<int>& dejaContenu, bool aAjouter);


	Grille& operator=(Grille &arg)
	{
		couvert=arg.couvert;
		capteurs=arg.capteurs;
		connecte=arg.connecte;
		coverGraph=arg.coverGraph;
		connectGraph=arg.connectGraph;
		nbCapteurs=arg.nbCapteurs;
		distance=arg.distance;
		inamovible=arg.inamovible;
	    return *this;
	}
	//recuit simulé sur l'heuristique destrcutive.
	void recuitSimule();
	//renvoie un nombre étant plus élevé quand la grille est plus structurée, entre 0 et 1
	double structureValue();
	double score(int T){return (double)nbCapteurs-5*T*structureValue();}
	string toString() const;
	//réinitialise toutes les variables statiques, pour changer d'instance du problème.
	static void reset();
	//recuit multi_thread
	void mtrecuit();
	void thread_recuit(array<bool,8> & flag_this, array<bool,8> & flag_T,double & T, Grille & best,int & cnt, mutex & m, int id);
};
