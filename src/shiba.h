// SHIBA: Stochastic Historical Biogeography Analysis  
// Cam Webb <cwebb@oeb.harvard.edu>
// $Revision: 1.9 $

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "land.h"

// NB - for surv, reduce with too much occ, AND increase with too few runs to pres

// Case definition:

#define MAXLINEAGES 1000 // For dimensioning arrays - large for simulations

#include "lineage.h"


// Output form:
#define VERBOSE 0         // Output form:
                           //   1 - minimal

// For solving:

#define MAXRUNS 1000000000  // How long to run the prog for
#define SUCCESSES 1000       // Stop after this many successes
#define P_SURV 1.0         // The survival modifier
#define P_DISP 0.2         // The dispersal modifier
#define S_DISP 1.0         // Shape of the dispersal curve (set exp ca. -0.003) 

// The number of areas in which the lineage starts
#define STARTAREAS 1

// For generating phylos (mmain)
#define P_SPECIATE 0.2  // Speciation rate
#define RNDSTART 1      // Random location start?
#define START_IN 0      // if not, start where?

// Global variables

float survp = P_SURV;
float dispp = P_DISP;
float dispp2 = 10; // a in a^(-b * dist)

// The spatial location of each lineage at each time 1 or 0
int locn[MAXLINEAGES][TIME][SPACE];

int record[MAXLINEAGES][SPACE]; 
// remember this records node position - not final position

int success = 0;
int topres1000 = 0;
int topresent = 0;

// for generating phylos

int starting[SPACE];
int ending[SPACE]; // totals
int end[SPACE]; // in loop
int bpa[MAXLINEAGES][SPACE];
int lineageCnt;
float pSpeciate = P_SPECIATE;

int maxdist;
int maxarea;
float obsocc = 0.0;
int extntlin = 0;  // N living lineages;
float occrunning = 0.0;

// Functions

// solve
void Loop();
int printArray();
int printExtant();
int printSuccess();
int printSuccessAll();
float pDisp();
float pSurv();
int findMaxArea();
int findMaxDist();
void printWelcome();
// make
int mmain();
void mLoop();
int mprintSuccess();
int printMembers();
int outputBPA();
void ObsOcc();


// TO DO: 
// 1. Allow speciation and dying out that still results in correct censored 
//    tree
// 2. Test no of simulated occurrences, and adjust pDisp and pMort
//    to optimize
