#include "Grille.hpp"
#include <random>
#include <list>
#include <algorithm>
#include <functional>
#include <ctime>
using namespace std;
//générateur aléatoires thread safe
array<minstd_rand0,8>  gen;
void Grille::mtrecuit()
{
    mutex m;
    Grille best(taille,rCapt,rCom);
    best=*this;
    array<bool,8> flag_this; //flag de modification de this
    array<bool,8> flag_T; //falg de modification de la température
    int cnt=0;
    double T=1; //température initiale
    for(int i=0; i<8; i++) //initialisations
    {
        flag_this[i]=false;
        flag_T[i]=false;
        gen[i]=minstd_rand0(rand());
    }
    list<thread> t;
    for(int i=0; i<8; i++) //lancement des 8 theads.
    t.emplace_back(&Grille::thread_recuit,this,ref(flag_this),ref(flag_T),ref(T),ref(best),ref(cnt),ref(m),i);
    for(auto i=t.begin(); i!=t.end(); i++)//attente de la fin des thread
    i->join();
    *this=best;
}

void Grille::thread_recuit(array<bool,8> & flag_this, array<bool,8> & flag_T,double & T, Grille & best,int & cnt, mutex & m, int id)
{
    double rho=0.85;
    int k;
    int cntcapt=0, cntempty=0;
    bool stop_flag=true;
    int N;
    vector<int> capt;
    capt.resize(taille*taille-1);
    vector<int> temp_capt;
    temp_capt.resize(taille*taille-1);
    vector<int> empty;
    empty.resize(taille*taille-1);
    vector<int> temp_empty;
    temp_empty.resize(taille*taille-1);
    m.lock();
    //////// section critique : initialisation des listes de capteurs
    for(int i=1; i<taille*taille; i++)
    if(!capteurs.test(i))
    {
        empty[cntempty]=i;
        cntempty++;
    }
    else
    {
        capt[cntcapt]=i;
        cntcapt++;
    }
    N=nbCapteurs;
    k=N*0.25;
    //////////
    Grille g(taille, rCapt,rCom);
    m.unlock();
    while(stop_flag)
    {
        for(int i=0; i<k; i++)
        {
            k=ceil(N*0.25);
            int pivot=gen[id]()%N; //tirage au sort du capteur à enlever
            //shuffle des listes de capteurs et de non capteur :  pas certain que ce soit thread safe mais j'ai pas eu de problèmes
            random_shuffle(empty.begin(),empty.begin()+taille*taille-1-N);
            random_shuffle(capt.begin(),capt.begin()+N);
            g.pivotDestructeur(capt,empty,temp_capt,temp_empty,capt[pivot]);//génération de la nouvelle solution
            m.lock(); /////// SECTION CRITIQUE
            double lambda=exp((double)(N-g.getNbCapteurs())/T);
            int p=100*lambda;
            if(g.getNbCapteurs()<best.getNbCapteurs()) // si on a trouvé une solution améliorante on met a jour
            {
                capt=temp_capt;
                empty=temp_empty;
                *this=g;
                for(int j=0; j<8; j++)
                if(j!=id)
                flag_this[j]=true;
                cnt=0;
                best=g;
                cout<<"new best"<<best.getNbCapteurs()<<" by thread "<<id<<endl;
                N=best.getNbCapteurs();

            }
            if(flag_this[id]) //si la solution a été modifiée on abandonne notre calcul et on prend la nouvelle solution
            {
                flag_this[id]=false;
                cntempty=0;
                cntcapt=0;
                for(int i=1; i<taille*taille; i++)
                if(!capteurs.test(i))
                {
                    empty[cntempty]=i;
                    cntempty++;
                }
                else
                {
                    capt[cntcapt]=i;
                    cntcapt++;
                }
                while(capt[cntcapt]!=0)
                {
                    capt[cntcapt]=0;
                    cntcapt++;
                }
                while(empty[cntempty]!=0)
                {
                    empty[cntempty]=0;
                    cntempty++;
                }
                N=nbCapteurs;
            }
            else if(rand()%100<p) // sinon si on veut modifier la solution on le fait.
            {
                capt=temp_capt;
                empty=temp_empty;
                *this=g;
                for(int j=0; j<8; j++)
                if(j!=id)
                flag_this[j]=true;
                N=nbCapteurs;
            }
            m.unlock();
            ////////////
        }
        //SECTION CRITIQUE
        m.lock();
        if(flag_T[id])
        flag_T[id]=false;
        else //si la température n'a pas déja changé on peut la changer
        {
            cnt++;
            T*=rho;
            cout<<"T="<<T<<endl;
            for(int j=0; j<8; j++)
            if(j!=id)
            flag_T[j]=true;
        }
        stop_flag=T>0.1 || cnt<2; //on regarde si on s'arrete
        m.unlock();
        ////////
    }
}
