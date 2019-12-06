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
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

namespace k_means
{
  //affiche les points en leur donnant une couleur differente selon leur blob
  //affiche egalement les centres mu
  auto affiche_blob(const std::vector<std::vector<double>>& vecteur_de_vecteur_de_x,
		    const std::vector<std::vector<double>>& vecteur_de_vecteur_de_y,
		    const std::vector<double>& v_centre_x,
		    const std::vector<double>& v_centre_y)
  {
    
    for(int i = 0 ; i<vecteur_de_vecteur_de_x.size() ; i++)
     {
	
       plt::scatter(vecteur_de_vecteur_de_x[i],
		    vecteur_de_vecteur_de_y[i]);
	
     }

    //le centre peut parfois avoir la meme couleur que le blob, donc on ne le voit pas forcement
    plt::plot(v_centre_x,v_centre_y,"o");
    plt::show();
  }



  
  //initialise de maniere aleatoire selon une distribution uniforme, le vecteur contenant les centres mu
  //la taille des vecteurs doit être reservé dans le main a partir de argv[]
  auto init_vecteur_centre(std::vector<double>& vecteur_centre_x,
			   std::vector<double>& vecteur_centre_y)
 
  {
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0,10.0);
    for(int i = 0 ; i<vecteur_centre_x.size() ; i++ )
       { 
	 
	 vecteur_centre_x[i]   = distribution(generator);
	 vecteur_centre_y[i]   = distribution(generator);
	
	 }
  }



  
  //associe des points a un cluster càd rempli le vecteur correspondant au centre auquel on associe un point du plan
  auto find_indice_du_min(const std::vector<double>& table_distance)
  {
   double temp = table_distance[0];
   int indice  = 0;
   for(int i=0; i<table_distance.size(); i++)
     {
      if(temp>table_distance[i])
	{
         temp=table_distance[i];
	 indice=i;
	}
     }
   return indice;
  }


  
  auto calcul_distance_et_indice_du_min(const double pos_x,
					const double pos_y,
					const std::vector<double>& vecteur_centre_x,
					const std::vector<double>& vecteur_centre_y,
					std::vector<double>& table_distance )
  {
    for(int i = 0 ; i < vecteur_centre_x.size() ; i++ )
      {
	//calcul distance euclidienne
	table_distance[i]=sqrt( (vecteur_centre_x[i]-pos_x)*(vecteur_centre_x[i]-pos_x) +(vecteur_centre_y[i]-pos_y)*(vecteur_centre_y[i]-pos_y)  );
      }
    
    double indice_du_min=find_indice_du_min(table_distance);
    return indice_du_min;
  }


  
  auto association_a_un_centre(const std::vector<double>& vecteur_de_vecteur_de_x,//donnees "brute"
			       const std::vector<double>& vecteur_de_vecteur_de_y,
			       const std::vector<double>& vecteur_centre_x,
			       const std::vector<double>& vecteur_centre_y,
			       std::vector<double>& table_distance,
			       std::vector<std::vector <double>>& vect_de_vect_x,//associer a une couleur
			       std::vector<std::vector <double>>& vect_de_vect_y)
  {
   
	for(int j = 0 ; j<vecteur_de_vecteur_de_x.size();j++)//pour tout les elements
	  {
	    int ret = calcul_distance_et_indice_du_min(vecteur_de_vecteur_de_x[j],
						       vecteur_de_vecteur_de_y[j],
						       vecteur_centre_x,
						       vecteur_centre_y,
						       table_distance);
	    
	    vect_de_vect_x[ret].push_back(vecteur_de_vecteur_de_x[j]);
	    vect_de_vect_y[ret].push_back(vecteur_de_vecteur_de_y[j]);
	    //attention il faut absolument tout supprimer sinon le vecteur grossi grossi grossi.. C'est pourquoi il y a une fonction "nettoyage"
	  }

  }



  
  //calcule un "centre de masse"
  auto calcule_position_centre(const std::vector<double>& coordonnees)
  {
    double ret=0;
    for(int i = 0 ; i < coordonnees.size() ; i++ )
      {
	ret += coordonnees[i];
      }
    double nb_points = coordonnees.size();
    if(nb_points > 0)
      {
      return (ret/nb_points);
      }
    
    else
      {
      //si aucun point n'a été associé a un centre, on lui donne une nouvelle position de maniere aleatoire
      //on evite ainsi une division par zero
      const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::default_random_engine generator(seed);
      std::normal_distribution<double> distribution(0.0,10.0);
      return distribution(generator);
      }
  }



  
  //rempli les vecteurs contenant les abscisses et ordonnees des centres 
  auto calcul_les_nouveaux_centres(std::vector<double>& vecteur_centre_x,//position des centres en x
				   std::vector<double>& vecteur_centre_y,//positions des centres en y 
				   const std::vector<std::vector <double>>& vect_de_vect_x,//vecteur contenant des vecteurs contenant les coords
				   const std::vector<std::vector <double>>& vect_de_vect_y)//des points associes a une couleur
    
  {
    for(int i = 0 ; i < vecteur_centre_x.size() ; i++)
      {
	    vecteur_centre_x[i]=calcule_position_centre(vect_de_vect_x[i]);
	    vecteur_centre_y[i]=calcule_position_centre(vect_de_vect_y[i]);
	    
	   
	 
      }
  }


  
  auto nettoyage(std::vector<std::vector <double>>& vect_de_vect_x,
		 std::vector<std::vector <double>>& vect_de_vect_y)
  {
    for(int i = 0 ; i < vect_de_vect_x.size(); i++)
      {
	vect_de_vect_x[i].clear();
	vect_de_vect_y[i].clear();
      }
  }
};

