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
float **Area ; ///< The areas of spatial time Area[Time][Space]
int Phylos;
int Taxa;
char **Taxon;
char **TaxonLabel;

// ------------------------ FUNCTIONS ---------------------------

void readXML();
void error(char *a);

/*!

 \todo Allow speciation and dying out that still results in correct censored 
       tree

 \todo Test no of simulated occurrences, and adjust pDisp and pMort
       to optimize
*/
