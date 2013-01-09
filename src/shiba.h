// This software is distributed under the terms of the BSD 2-Clause License
// (see bottom of this file for full text)

/*! \file
 * **SHIBA main header**
*/

#define _GNU_SOURCE //!< to allow the very useful asprintf()

/*! Safer asprintf macro for extending strings.
 * From [21st Century C](http://shop.oreilly.com/product/0636920025108.do).
 */
#define Sasprintf(write_to, ...) {          \
  char *tmp_string_for_extend = (write_to); \
  asprintf(&(write_to), __VA_ARGS__);       \
  free(tmp_string_for_extend);              \
  }

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for getopt()
#include <ctype.h>  // for isprint()
#include <math.h>
#include <string.h>
#include <time.h>   // for time()
#include "version.h"

// ------------------------ STRUCTURES ---------------------------

//! The structure that stores the configuration information
typedef struct {
  //! 1/0 defining if stem lineage is allowed to start in each space
  int *startSpace; 
  int nStartSpaces; //<! The number of starting spaces
  int maxRuns;
  int stopAfterSuccess;
  double probSurv;
  double probDispA;
  double probDispB;
  double shapeDisp;
  int verbose;
} config;

//! A general phylogeny structure returned from pasring Newick
typedef struct {
  int nnodes;  //!< Number of nodes in phylogeny, including 0 or root node
  int *parent; //!< index of parent node
  int *ndaughter; //!< Number of daughters for each node. Terminals have zero
  int *depth; //!< Depth in n nodes from root (root has depth of zero)
  double *bl; //!< Branch length of the proximal edge
  int ultra;  //!< 0/1 flagging an ultrametric tree.
  double *age; //!< \brief Age of the node, in units of branch length
               //!< (valid only for ultrametric trees).
  char **taxon; //!< Name of the node, NULL for missing names
} phylo;

// ------------------------ VARIABLES ---------------------------

//! The main config structure
config Cfg;

int Times;        //!< The number of time periods 
double *RealTime; //!< The _age_ of the start of the period

int Spaces;       //!< The number of spatial units
char **SpaceName; //!< The names of the spatial area

int Phylos;       //!< The number of phylogenies
char **Phylo;     //!< The phylogeny strings

int Taxa;         //!< The number of terminal taxa
char **Taxon;     //!< Terminal taxa names

double **Area ;   //!< The areas of spates at each time: Area[Time][Space]
double ***Dist ;  //!< The distances among spaces at each time 
int    ***Extant; //!< \brief 0/1 indcating the existance of a taxon or fossil
                  //!< during a particular time period.  Care is needed in
                  //!< assigning fossils to periods, due to reconciliation
                  //!< stage. [x][t][s]

int    Lineages;     //!< The number of lineages = phylo.nnodes
int  **LineagePeriod; //!< The 0/1 existence of a lineage in a time period.
int  **LineageDaughters; //!< The daughters of each lineage.
int   *LineageDaughtersN; //!< The number of daughters of each lineage.
int ***LineageExtant; //!< Whether each lineage is extant. [l][t][s]
int    LinExtantN;    //!< The number of lineages extant at present

char *DataFile;   //!< Name of the data file. Default: `shibaInput.xml`
int PhyloToUse;   //!< Index number of the phylogeny currently in use.
int PrintData;    //!< Switch 0/1 to control output of raw data.

// #### these variables should be renamed and their scope restricted:
// The spatial location of each lineage at each time 1 or 0
int ***locn; //[MAXLINEAGES][TIME][SPACE];
int **record; // [MAXLINEAGES][SPACE]; remember this records node position - not final position
long int success;
long int topresent;
double maxdist;
double maxarea;



// ------------------------ FUNCTIONS ---------------------------
// (documentation precedes the function definitions)

void readArgs(int argc, char **argv);
void readXML();
void printIndata();
phylo parseNewick(char *in);
void phyloToLineage(phylo p);
void help();
void error(char *a);

int* mem1d_i(int dimx);
double* mem1d_d(int dim);

int** mem2d_i(int dimx, int dimy);
void free2d_i(int **ptr, int dimx);
int** mem2d1_i(int dimx);
void free2d1_i(int **ptr, int dimx);
double** mem2d_d(int dimx, int dimy);
void free2d_d(double **ptr, int dimx);
char** mem2d1_c(int dimx);
void free2d1_c(char **ptr, int dimx);

double*** mem3d_d(int dimx, int dimy, int dimz);
void free3d_d(double ***ptr, int dimx, int dimy);
int*** mem3d_i(int dimx, int dimy, int dimz);
void free3d_i(int ***ptr, int dimx, int dimy);

void shiba(phylo p);
double findMaxArea();
double findMaxDist();
void biogeo();
void printSuccessAll(phylo p);
double pDisp(int t, int a, int b);
double pSurv(int t, int a);
void printArray(int t);

/* TODO: Allow speciation and dying out that still results in correct censored 
 *       tree
 * TODO: Test no of simulated occurrences, and adjust pDisp and pMort
 *       to optimize
 */


/* Copyright (c) 2013, Campbell Webb
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
   OF THE POSSIBILITY OF SUCH DAMAGE.
 */
