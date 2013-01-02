/*!
  SHIBA main header
*/

#define _GNU_SOURCE // to allow the very useful asprintf()
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "nrutil.h"

// ------------------------ STRUCTURES ---------------------------

/*!
 * The structure that stores the configuration information
 */

typedef struct shibaConfig {
  int time;
  int space;
  char **spacenames;
} shibaConfig;

// ------------------------ VARIABLES ---------------------------

shibaConfig cfg;

int Times;  ///< The number of time periods 
char **TimeLabel; ///< The ID label in the XML file for the time period
double *RealTime;
int Spaces; ///< The number of spatial units
char **SpaceName;
char **SpaceLabel;
char **Phylo;
char **PhyloLabel;
double **Area ; ///< The areas of spatial time Area[Time][Space]
double ***Dist ; ///< 
int    ***Extant; 
int Phylos;
int Taxa;
char **Taxon;
char **TaxonLabel;

// ------------------------ FUNCTIONS ---------------------------

void readXML();
void error(char *a);
double* mem1d_d(int dim, char *msg);
//int mem2d_d(double ***ptr, int dimx, int dimy, char *msg);
double** mem2d_d(int dimx, int dimy, char *msg);
void free2d_d(double **ptr, int dimx);
char** mem2d1_c(int dimx, char *msg);
double*** mem3d_d(int dimx, int dimy, int dimz, char *msg);
void free3d_d(double ***ptr, int dimx, int dimy);
int*** mem3d_i(int dimx, int dimy, int dimz, char *msg);
void free3d_i(int ***ptr, int dimx, int dimy);


/*!

 \todo Allow speciation and dying out that still results in correct censored 
       tree

 \todo Test no of simulated occurrences, and adjust pDisp and pMort
       to optimize
*/
