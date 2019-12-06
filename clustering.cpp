#include <cmath>
#include <random>
#include <chrono>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "k_means.cpp"
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;


namespace my_dataset_gen

{
  //vecteur contient toutes les abscisses(ou ordonnes) du nuage de points les uns a la suite des autres
  auto random_gen(std::vector<double>& vecteur,
		  const int begin_pos,
		  const int end_pos  )
  {
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(0,10);
    std::normal_distribution<double> distribution_2(distribution(generator),1);
    for(int i=begin_pos;i<end_pos;i++)
      {
	vecteur[i]=distribution_2(generator);
      }
  }
}

//modifie la taille des vecteurs en fonction des arguments donnes par l'utilisateur grace a argv[]
namespace blob_creator
{
  auto allocate_blob(const int nb_blob,
		     const int nb_pts_per_blob,
		     std::vector<double>& vect_des_x,
		     std::vector<double>& vect_des_y
		     )
  {
    vect_des_x.resize(nb_blob * nb_pts_per_blob);
    vect_des_y.resize(nb_blob * nb_pts_per_blob);
       
  }

  //cree le nuage de point de maniere aleatoire 
  auto create_blob(const int nb_blob,
		   const int nb_pts_per_blob,
		   std::vector<double>& vect_des_x,
		   std::vector<double>& vect_des_y)
  {
    int j = 0;
    for(int i=0 ; i<nb_blob * nb_pts_per_blob ; )//nb_blob tour de boucle
      {
	
	my_dataset_gen::random_gen(vect_des_x,i,i+nb_pts_per_blob);
	
	
	my_dataset_gen::random_gen(vect_des_y,i,i+nb_pts_per_blob);
	j++;
	i=i+nb_pts_per_blob;
      }
  }
}


int main(int argc,char* argv[])
{
  
  
  const int nb_blob=atoi(argv[1]);
  const int nb_pts_per_blob=atoi(argv[2]);
  const int nb_centre=atoi(argv[3]);

  std::vector<std::vector<double>> vecteur_de_blob(2*nb_blob,//x_1,y_1  x_2,y_2  x_3,y_3 x_nb_blob,y_nb_blob
						   std::vector<double>(nb_pts_per_blob));
  std::vector<double> vecteur_x(0);
  std::vector<double> vecteur_y(0);
  
  std::vector<double>& v_x = vecteur_x;
  std::vector<double>& v_y = vecteur_y;
  std::vector<std::vector<double>>& vect_de_vect = vecteur_de_blob;
  
  
 
  blob_creator::allocate_blob(nb_blob,
			      nb_pts_per_blob,
			      v_x,
			      v_y);

  blob_creator::create_blob(nb_blob,
			    nb_pts_per_blob,
			    v_x,
			    v_y);
  //on declare des vecteurs contenant, l'un les abscisses des centres,
  //l'autres les ordonnees des centres
  std::vector<double> v_centre_x(nb_centre);
  std::vector<double> v_centre_y(nb_centre);

  //on cree des reference pour ameliorer la perf du code
  std::vector<double>& vecteur_centre_x = v_centre_x;
  std::vector<double>& vecteur_centre_y = v_centre_y;

  //on choisit aleatoirement des coordonnees pour les centres
  k_means::init_vecteur_centre(vecteur_centre_x,
			       vecteur_centre_y);

  //ce vecteur contiendra les distances entre un point du nuage de point et chacun des centres que l'on a choisit
  //ce vecteur est utilise dans la fonction association_a_un_centre
  std::vector<double> table_distance(nb_centre);
  std::vector<double>& ref_table=table_distance;

  //on cree un vecteur qui contiendra un vecteur contenant les abscisse des points associes au centre numero "indice_dans_le_vecteur"
  std::vector<std::vector<double>> vect_de_vect_de_x_associe_centre(nb_centre,std::vector<double>(0));
  //idem pour les ordonnees
  std::vector<std::vector<double>> vect_de_vect_de_y_associe_centre(nb_centre,std::vector<double>(0));

  
  //on utilise reserve car on ne sait pas à l'avance combien de point chaque centre va "recuperé"
  //pourquoi autant de place ? a priori, un centre pourrait tres bien recuperer tous les points de l'espace.
  for(int i = 0 ; i <nb_centre ; i++)
    {
  vect_de_vect_de_x_associe_centre[i].reserve(nb_blob * nb_pts_per_blob);
  vect_de_vect_de_y_associe_centre[i].reserve(nb_blob * nb_pts_per_blob);
  }
  
  //reference pour faire des passage de donnees par reference
  auto& ref_vect_vect_x =vect_de_vect_de_x_associe_centre;
  auto& ref_vect_vect_y =vect_de_vect_de_y_associe_centre;

  //on fait 10 cycles "d'association de points à un centre"
  for(int i = 0 ; i<10 ; i++)
    {
      
      
      k_means::association_a_un_centre(v_x,
			      v_y,
			      vecteur_centre_x,
			      vecteur_centre_y,
			      table_distance,
			      ref_vect_vect_x,
			      ref_vect_vect_y);


      //remarque, il n'y a pas autant de couleur que l'on veut,
      //matplotlib boucle sur  environ 10 couleurs differentes
      k_means::affiche_blob(ref_vect_vect_x, //affiche les cluster
			    ref_vect_vect_y,
			    vecteur_centre_x,
			    vecteur_centre_y);//affiche les centres

       
      k_means::calcul_les_nouveaux_centres(vecteur_centre_x,
				   vecteur_centre_y,
				   ref_vect_vect_x,
				   ref_vect_vect_y);
      

      //cette fonction utilise la methode clear des std::vector
      //la maniere dont on a conçu  le programme nous y oblige
      //si on le fait pas, chaque point sera associé plusieur fois
      //en effet, on n'ecrase aucune valeur, on fait des push_back
      k_means::nettoyage(ref_vect_vect_x,
			 ref_vect_vect_y);

      //on re-reserve suffisemment de place et on reboucle
      for(int i = 0 ; i <nb_centre ; i++)
    {
  vect_de_vect_de_x_associe_centre[i].reserve(nb_blob * nb_pts_per_blob);
  vect_de_vect_de_y_associe_centre[i].reserve(nb_blob * nb_pts_per_blob);
  }
    }


}


