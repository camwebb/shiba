/*!
  SHIBA main header
*/

#define _GNU_SOURCE // to allow the very useful asprintf()
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// ------------------------ STRUCTURES ---------------------------

/*!
 * The structure that stores the configuration information
 */

typedef struct {
  int *startSpace;
} shibaConfig;

// ------------------------ VARIABLES ---------------------------

shibaConfig Cfg;

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
void printIndata();

void error(char *a);

int* mem1d_i(int dimx);
double* mem1d_d(int dim);

double** mem2d_d(int dimx, int dimy);
void free2d_d(double **ptr, int dimx);
char** mem2d1_c(int dimx);

double*** mem3d_d(int dimx, int dimy, int dimz);
void free3d_d(double ***ptr, int dimx, int dimy);
int*** mem3d_i(int dimx, int dimy, int dimz);
void free3d_i(int ***ptr, int dimx, int dimy);


/*!

 \todo Allow speciation and dying out that still results in correct censored 
       tree

 \todo Test no of simulated occurrences, and adjust pDisp and pMort
       to optimize
*/
