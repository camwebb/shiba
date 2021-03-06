/*! \file
 * **Biogeographical routines**
 */

#include "shiba.h"

void shiba(phylo phy)
{
  // pthreads preparation:
  pthread_t *thread = (pthread_t *) malloc(RUN_BATCH * sizeof(pthread_t));
  pthread_attr_t attr;
  void *status; // for pthread_join
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // these are global variables passed between functions. 
  // Shrink the scope if poss, and rename
  record = mem2d_i(Lineages, Spaces);
  success = 0;
  topresent = 0;
  maxarea = findMaxArea();
  maxdist = findMaxDist();

  // set disp, surv functions
  PSurv = mem2d_d(Times, Spaces);
  PDisp = mem3d_d(Times, Spaces, Spaces);
  for (int t = 0; t < Times; t++)
    for (int s = 0; s < Spaces; s++) {
      PSurv[t][s] = pSurvival(Area[t][s] /  maxarea);
      for (int s2 = 0; s2 < Spaces; s2++) 
        PDisp[t][s][s2] = pDispersal(Dist[t][s][s2] /  maxdist);
    }
  
  // initialize srandom with time
  srandom(time(NULL));

  printf("\n# SHIBA (Simulated Historical Island Biogeography Analysis)\n\n");
  
  long int batch;
  // run in batches of RUN_BATCH 
  for (batch = 0; batch < (long int)(Cfg.maxRuns/RUN_BATCH);batch++) {
    // start threads
    for (long int i = 0; i < RUN_BATCH ; i++)
      pthread_create(&thread[i], &attr, biogeo, NULL);

    // collect:
    for (long int i = 0; i < RUN_BATCH ; i++)
      pthread_join(thread[i], &status);

    // Reporting (and adjustment)
    fprintf(stderr,"\r> runs: %9ld;    to present: %8ld;    successes: %5ld", 
            batch * RUN_BATCH , topresent , success);
    fflush(NULL);  // Flush the buffer

    // check for successes
    if (success >= Cfg.stopAfterSuccess) break;
  }

  // Final reporting:
  fprintf(stderr, "\n\n"); fflush(NULL);
  printf("## Successes : %ld (in %ld runs)\n\n", success, batch * RUN_BATCH );
  printSuccessAll(phy);

  // free3d_i(locn , Lineages, Times);
  free2d_i(record, Lineages);

  free(thread); free(status);
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mymutex);

  //! \page prog Programming Notes
  //! ## Memory leaks
  //! `pthreads` itself leaves a few unfreed but not leaking mallocs behind. 
  //! Running `valgrind --leak-check=full --show-reachable=yes` gives some 
  //! 'still reachable' bytes lost. This is not a problem. 
  //! See <http://stackoverflow.com/questions/3844678/pthread-exit-vs-return>
}

double findMaxArea()
{
  // finds total area on the landscape
  // was - max island size.  Now unlikely that any area get 100% survival
  double tot, max = 0.0;
  for (int t = 0; t < Times; t++) {
    tot = 0;
    for (int a = 0; a < Spaces; a++) tot += Area[t][a];
    if (tot > max) max = tot;
  }
  return max;
}

double findMaxDist()
{
  double max = 0.0;
  for (int t = 0; t < Times; t++) 
    for (int a = 0; a < Spaces; a++) 
      for (int b = 0; b < Spaces; b++) 
		if (Dist[t][a][b] > max) max = Dist[t][a][b];
  return max;
}

void *biogeo()
{

  int ***locn = mem3d_i(Lineages, Times, Spaces);

  //! \page pseudo Pseudocode
  //! ## Main biogeographic loop
  //! * For each run...

  //! \page pseudo Pseudocode 
  //! ### Establish starting positions for Lineage 0 
  //! * Until the allowed number of starting spaces (`Cfg.nStartSpaces`)
  //!   for lineage 0 have been chosen...

  int ss = 0;
  while (ss < Cfg.nStartSpaces) {
    int rnd = (int)((double) Spaces * (double) random() /
                    (double)(RAND_MAX+1.0));

    //! \page pseudo Pseudocode
    //! * Pick a random space. 
    //!
    //! * If i) space has not already been chosen, ii) area is > 0, i.e., 
    //!   there is land, and iii) it is an allowed start spot, 
    //!   set that start space for lineage 0.

    if ((!locn[0][0][rnd]) && 
        (Area[0][rnd] > 0.0) && 
        (Cfg.startSpace[rnd])) {
      locn[0][0][rnd] = 1;
      ss++;
    }
  }

  //! \page pseudo Pseudocode
  //! ### Main loop from time 0 to present
  //! * For each period (`t`) up until the penultimate one (because actions
  //!   cause changes in state at `t+1`)...
  for (int t = 0; t < Times-1; t++)  { 
    // print array at start
    if (Cfg.verbose==1) 
      for (int i = 0; i < Lineages; i++)
        if (LineagePeriod[i][t]) {
          printf("t%d l%d : ", t, i);
          for (int x = 0; x < Spaces; x++) printf("%d", locn[i][t][x]);
          printf("\n");
        }
  
    //! \page pseudo Pseudocode
    //! * For each lineage... (`l`)
    for (int l = 0; l < Lineages; l++) {

      //! \page pseudo Pseudocode
      //! * Exclude those lineages not alive at period `t`
      if (LineagePeriod[l][t]) {

        int linck = 0; // the number of surv lineages from one period to next
        
        //! \page pseudo Pseudocode
        //! * For each space... (`s`)
        for (int s = 0; s < Spaces; s++) {

          //! \page pseudo Pseudocode
          //! * (Optionally test for fossils: if there is a fossil in
          //!   a place and no lineage in that place, this causes 
          //!   a failure)

          //! \page pseudo Pseudocode
          //! * Where there is a lineage in a space at a  time (set either
          //!   from initialization or from the previous run)...
          if (locn[l][t][s]) {

            //! \page pseudo Pseudocode
            //! * Test for **dispersal** to each other space (that is
            //!   not the same one and has Area > 0). 
            //!   If a randomly chosen 0...1 is less than the
            //!   distance-dependent prob of dispersal, set the (potential) 
            //!   lineage in that space in `t+1`
            for (int i = 0; i < Spaces ; i++) {
              if ( ( ((double) random() / (double)(RAND_MAX+1.0))
                     < PDisp[t][s][i] ) && (i != s) && (Area[t][i] > 0.0)) {
                locn[l][t+1][i] = 1;
                
                if (Cfg.verbose==1) printf("               l%d disperses from s%d to s%d\n",l,s,i);
                linck++;
              }
            }
            
            //! \page pseudo Pseudocode
            //! #### Survival and dispersal
            //! * Test for **survival in place**.
            //!   If a randomly chosen 0...1 is less than the
            //!   area-dependent prob of survival, set the (potential) lineage
            //!   as alive in 'own' space in `t+1`
            if (((double) random() / (double)(RAND_MAX+1.0)) < PSurv[t][s]) {
              locn[l][t+1][s] = 1;
              linck++;
            }
            else if (Cfg.verbose==1) printf(
              "               l%d goes locally extinct in s%d\n", l , s);
            
          }
        }

        //! \page pseudo Pseudocode
        //! * (If no existences of this lineage make it to `t+1`, end this run
        //!   because the simulation cannot not make a match to the existing
        //!   distribution)
        if (!linck) {
          if (Cfg.verbose==1) printf("               l%d has died out\n",l);
          if (Cfg.verbose==1) printf("----------\n");
          free3d_i(locn, Lineages, Times);
          pthread_exit(NULL);
          //return;
        }


        //! \page pseudo Pseudocode
        //! #### Switch from parent to daughter lineages 'if it is time'
        //! * Test for existence of lineage in `t+1`. If it is to die...
        if (!LineagePeriod[l][t+1]) {

          //! \page pseudo Pseudocode
          //! * Calculate how many spaces (`z`) it would have been in.
          //!   (If it is only in one place, then speciation must be
          //!   **sympatric**, i.e., two or more daughter lineages exist 
          //!   together in the single space at `t+1`.
          //!   If it is in several places, then speciation could be 
          //!   **sympatric** or **allopatric**.)

          int places = 0;      //number of spaces occupied by daughters of `l`
          int placemapping[Spaces]; // mapping from 0..places to 'real' spaces
          for (int s = 0; s < Spaces; s++)
            if (locn[l][t+1][s]) {
              placemapping[places] = s;  
              places++;
            }
          int placeck[places]; // has the place got a lineage in it? 
          for (int h = 0; h < places; h++) placeck[h] = 0;
          int fullplaces = 0;       // number of places filled
          
          //! \page pseudo Pseudocode
          //! * For each daughter lineage (0...`LineageDaughtersN[l]`)
          
          for (int q = 0; q < LineageDaughtersN[l]; q++) {
            int placed = 0; // has this daughter been placed?
            while (!placed) {
              
              //! \page pseudo Pseudocode
              //! * Pick a random number (0...`z`). If that place has not been 
              //!   filled, use that place for the new lineage. If that place 
              //!   has been filled, and all places have been filled just use 
              //!   that place for the new lineage, but if all places have 
              //!   not been filled, try again.  This algorithm generates
              //!   sympatry with `places=1`, allopatry with `l = 2` and 
              //!   `places=2` and a mixture with other combinations.
              int rnd = (int) ( (double) places * (double) random() / 
                                (double) (RAND_MAX+1.0));
              if (placeck[rnd]) {
                if (fullplaces >= places) {
                  locn[LineageDaughters[l][q]][t+1][placemapping[rnd]] = 1;
                  placed = 1;
                  if (Cfg.verbose==1) printf(
                    "               l%d ends; l%d begins in s%d\n", l, 
                    LineageDaughters[l][q], placemapping[rnd]);
                }
              } else {
                locn[LineageDaughters[l][q]][t+1][placemapping[rnd]] = 1;
                placeck[rnd] = 1;
                placed = 1;
                fullplaces++;
                if (Cfg.verbose==1) printf(
                  "               l%d ends; l%d begins in s%d\n", 
                  l, LineageDaughters[l][q], placemapping[rnd]);

              }
            }
          }
        }
      }
    }
    //! \page pseudo Pseudocode
    //! * (End lineage loop `l`)
  }
  //! \page pseudo Pseudocode
  //! * (End time loop `t`)
  //!
  //! * If the loop did not bail out due to a dying lineage, we have a
  //!   fully simulated lineage.

  // final layout
  if (Cfg.verbose==1)
    {
      for (int i = 0; i < Lineages; i++)
        if (LineagePeriod[i][Times-1]) {
          printf("t%d l%d : ", Times-1, i);
          for (int x = 0; x < Spaces; x++) printf("%d", locn[i][Times-1][x]);
          printf("\n");
        }

      //printExtant(Lineages);
    }

  //! \page pseudo Pseudocode
  //! #### Check for conformation of simulated taxa distribution to extant
  //! * Loop through lineages and spaces at present and count the
  //!   number of differences between simulated and onserved distribution
  
  int diffs = 0;  // the count of differences between sim and obs
  for (int l = 0; l < Lineages; l++) {
    if (LineagePeriod[l][Times-1]) {
      for (int s = 0; s < Spaces; s++) {
        if (locn[l][Times-1][s] != LineageExtant[l][Times-1][s]) diffs++;
      }
    }
  }
  
  //! \page pseudo Pseudocode
  //! * If there is no difference in simulated and observed distribution,
  //!   we record the route taken through space-time 

  // lock mutex
  pthread_mutex_lock(&mymutex);
  topresent++;

  if (!diffs) {
    success++;
    for (int l = 0; l < Lineages; l++) {
      for (int t = 0; t < Times; t++) {
        if (LineagePeriod[l][t]) {
          for (int s = 0; s < Spaces; s++)
            record[l][s] += locn[l][t][s];
          break;
        }
      }
    }
  }
  pthread_mutex_unlock(&mymutex);

  if (Cfg.verbose==1) printf("-----------\n");

  free3d_i(locn, Lineages, Times);
  pthread_exit(NULL);
}


void printSuccessAll(phylo p)
{
  printf("## Number of runs in which the base of each lineage was in each space:\n   (*= extant distrib)\n\n");
  // printf("lin  bor   wsu   ngu   phi   luz\n");
  printf("    Space       |  ");
  for (int k = 0; k < Spaces; k++) printf(" %3d ", k);
  printf("\n");
  printf("    ------------+--");
  for (int k = 0; k < Spaces; k++) printf("-----");
  printf("\n");
      
  for (int i = 0; i < Lineages; i++) {
    printf("    Lineage %3d |  ", i);
    for (int k = 0; k < Spaces; k++) {
      if (LineageExtant[i][Times-1][k])
        printf(" %3d*", record[i][k]);
      else  printf(" %3d ", record[i][k]);
    }
    printf("  --> ");
    if(p.taxon[i]) printf("%s", p.taxon[i]);
    for(int l = 0; l < LineageDaughtersN[i]; l++)
      printf("l%d ", LineageDaughters[i][l]);
    printf("\n");
  }
  printf("\n");
}

double pDispersal(double x) {
  return Cfg.probDispA * powf( 10.0, -1.0 * Cfg.probDispB * x ) ;
}

double pSurvival(double x) {  
  return Cfg.probSurvA * (log10f(( x * ( powf(10, Cfg.probSurvB) -1.0 )) +1.0 ) 
                           / Cfg.probSurvB );
}


