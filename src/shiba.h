/*!
  SHIBA main header
*/

#define _GNU_SOURCE // to allow the very useful asprintf()
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

//Safer asprintf macro
#define Sasprintf(write_to, ...) {          \
  char *tmp_string_for_extend = (write_to); \
  asprintf(&(write_to), __VA_ARGS__);       \
  free(tmp_string_for_extend);              \
  }

// ------------------------ STRUCTURES ---------------------------

/*!
 * The structure that stores the configuration information
 */

typedef struct {
  int *startSpace;
} shibaConfig;

typedef struct {
  //int treeNum;
  int nnodes; // including 0 or root node
  int *parent; //up[node]
  //int *ldaughter;
  //int *rsister;
  int *ndaughter;
  int *depth; //depth[node]
  double *bl; //bl[node]
  //float *tbl;
  double *age; //age[node]
  char **taxon; // name of named node - taxon[node][]
  //int ntaxa;  // number of named nodes = total number of names
  //int termtaxa; // number of terminal taxa
  //char **taxalist; //names of terminal taxa - taxalist[0 to termtaxa-1][]
  //int *t2n;  //vector of node #s indexed by 0 to termtaxa-1, as taxalist
  //float **dist; // matrix of all node-to-node distances dist[node1][node2]
  //int arenotes; // 0 | 1
  //char **notes;
  // DA additions
  //int maxDepth;
  //int *ntip;
  //int *nint;
  //int **tiplist;
  //int **intlist;
  //int *upo; // up pass order
} phylo;


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
int **LineagePeriod;

// ------------------------ FUNCTIONS ---------------------------

void readXML();
void printIndata();

void error(char *a);

int* mem1d_i(int dimx);
double* mem1d_d(int dim);

int** mem2d_i(int dimx, int dimy);
void free2d_i(int **ptr, int dimx);
double** mem2d_d(int dimx, int dimy);
void free2d_d(double **ptr, int dimx);
char** mem2d1_c(int dimx);
void free2d1_c(char **ptr, int dimx);

double*** mem3d_d(int dimx, int dimy, int dimz);
void free3d_d(double ***ptr, int dimx, int dimy);
int*** mem3d_i(int dimx, int dimy, int dimz);
void free3d_i(int ***ptr, int dimx, int dimy);

phylo parseNewick(char *in);
void phyloToLineage(phylo p);

/*!

 \todo Allow speciation and dying out that still results in correct censored 
       tree

 \todo Test no of simulated occurrences, and adjust pDisp and pMort
       to optimize
*/
