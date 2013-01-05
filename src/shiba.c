/*! \file
 * **Biogeographical routines **
 */

#include "shiba.h"

void shiba()
{

  // these are global variables passed between functions. 
  // Shrink the scope if poss, and rename
  locn = mem3d_i(Lineages, Times, Spaces);
  record = mem2d_i(Lineages, Spaces);
  success = 0;
  topres1000 = 0;
  topresent = 0;
  occ1000 = 0.0;
  maxarea = findMaxArea();
  maxdist = findMaxDist();

  // printf("Maxarea: %f\n", maxarea);
  // printf("Maxdist: %f\n", maxdist);

  //printf("SETTINGS\n");
  //printf("| surv = %5.3f |\n", Cfg.probSurv);
  obsocc = obsOccurrence();
  //printf("| occ  = %5.3f |\n",obsocc);
  //printf("+--------------+\n");

  // initialize srandom with time
  srandom(time(NULL));

  // clear counter variables
  long int r = 0; // runs

  // keep going until...
  while ((success < Cfg.stopAfterSuccess) && (r < Cfg.maxRuns))
    {

      biogeo();
      r++;

      // Reporting and adjustment every 1000 runs:
      if (!(r % 1000))
		{
		  // adjust base
		  // if ((occ1000 / (double) topres1000) > obsocc) dispp2 += 0.05;
		  // if ((occ1000 / (double) topres1000) < obsocc) dispp2 -= 0.05;

		  // adjust modifier
		  // 2009-09-13 drop:
		  //   if ((occ1000 / (double) topres1000) > obsocc) dispp -= 0.05;
		  //   if ((occ1000 / (double) topres1000) < obsocc) dispp += 0.05;
		  //   if (dispp > 1.0) dispp = 1.0;
		  //   if (dispp < 0.0) dispp = 0.0;

		  // Adjust surv
		  //if (topres1000 > lasttp1000) survp += 0.05;
		  //if ((occ1000 / (double) topres1000) < obsocc) dispp2 -= 0.05;

 		  /* fprintf(stderr, 
             "run %8ld; to present %8ld; success %8ld; occ %5.3f; disp %5.2f", 
				  r, (topresent - (topresent % 1000)), success, 
				  occ1000 / topres1000, Cfg.probDispA) ;
		  fflush(NULL);  // Flush the buffer
          */

		  occ1000 = 0.0;
		  topres1000 = 0;
          
		}
    }

  // Final reporting:
  fprintf(stderr, "\n"); fflush(NULL);
  printf("success / runs = %ld/%'ld\n", success, r );
  printSuccessAll();

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

double obsOccurrence()
{
  int areaCnt;
  double tmptot = 0.0;
  for (int i = 0; i < Lineages; i++)
    // for lineages that are extant at present:
    if (LineagePeriod[i][Times-1]) {
      areaCnt = 0;
      for (int j = 0; j < Spaces; j++)
        // if this lineage present in space j
        if (LineageExtant[i][Times-1][j]) 
          areaCnt++;
      LinExtantN++; // the number of lineages at pres
      tmptot += (double) areaCnt / (double) Spaces;
    }
  return (double) tmptot / (double) LinExtantN;
}

void biogeo()
{

  //! \page pseudo Pseudocode
  //! ## Main biogeographic loop
  //! * For each run...

  // clear 
  for (int l = 0; l < Lineages; l++)
    for (int t = 0; t < Times; t++)
      for (int s = 0; s < Spaces; s++)
		locn[l][t][s] = 0;

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
    if (Cfg.verbose==1) printArray(t);
    
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
                     < pDisp(t,s,i) ) && (i != s) && (Area[t][i] > 0.0)) {
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
            if (((double) random() / (double)(RAND_MAX+1.0)) < pSurv(t,s)) {
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
          return;
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

  topresent++;
  topres1000++;

  // final layout
  if (Cfg.verbose==1)
    {
      printArray(Times-1);
      //printExtant(Lineages);
    }

  //! \page pseudo Pseudocode
  //! #### Check for conformation of simulated taxa distribution to extant
  //! * Loop through lineages and spaces at present and count the
  //!   number of differences between simulated and onserved distribution
  
  int diffs = 0;  // the count of differences between sim and obs
  double tmpocccnt = 0.0;
  for (int l = 0; l < Lineages; l++) {
    if (LineagePeriod[l][Times-1]) {
      int occcnt = 0;
      for (int s = 0; s < Spaces; s++) {
        if (locn[l][Times-1][s] != LineageExtant[l][Times-1][s]) diffs++;
        occcnt += locn[l][Times-1][s];
      }
      tmpocccnt += (double) occcnt / (double) Spaces;
      //  printf("run %f; LinExtantN %d; occcnt %d; tmpocccnt %f\n", occ1000, LinExtantN, occcnt, tmpocccnt);
    }
  }
  
  occ1000 += tmpocccnt / (double) LinExtantN;

  //! \page pseudo Pseudocode
  //! * If there is no difference in simulated and observed distribution,
  //!   we record the route taken through space-time 
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

    if (Cfg.verbose==0)
  	  fprintf(stderr, "\r%3ld",success); fflush(NULL);
  
    if (Cfg.verbose==2) printSuccess();
  }

  if (Cfg.verbose==1) printf("-----------\n");
    
}

void printArray(int time)
{
  for (int i = 0; i < Lineages; i++)
    if (LineagePeriod[i][time]) {
      printf("t%d l%d : ", time, i);
      for (int x = 0; x < Spaces; x++) printf("%d", locn[i][time][x]);
      printf("\n");
    }
}

/* int printExtant(int lins) */
/* { */
/*   int i,x; */
/*   printf("CUR\n"); */
/*   for (i = 0; i < lins; i++) */
/*     { */
/*       if (LineagePeriod[i][Times-1] == 1) */
/* 		{ */
/* 		  printf("    T%2d L%2d  ", realtime[Times-1], i); */
/* 		  for (x = 0; x < Spaces; x++) printf("%d", extant[i][x]); */
/* 		  printf("\n"); */
/* 		} */
/*     } */

/*   return 1; */
/* } */

void printSuccess()
{
  printf("int locn[Lineages][Times][Spaces] = { \\\n");
  for (int i = 0; i < Lineages; i++)
    {
      printf("{ \\\n");
      for (int j = 0; j < Times; j++)
		{
		  printf("  {%d", locn[i][j][0]);
		  for (int k = 1; k < Spaces; k++)
			{
			  printf(", %d", locn[i][j][k]);
			}
		  if (j < Times-1) printf("}, \\\n");
		  else printf("} ");
		}
      if (i < Lineages-1) printf("}, \\\n");
      else printf("} \\\n");
    }
  printf("};\n\n\n");

  /*   printf("YES (%d%d %d%d %d%d) %d%d %d%d %d%d %d%d\n", \ */
  /* 	 locn[0][1][0],  locn[0][1][1], \ */
  /* 	 locn[1][6][0],  locn[1][6][1], \ */
  /* 	 locn[2][9][0],  locn[2][9][1], \ */
  /* 	 locn[3][Times-1][0], locn[3][Times-1][1], \ */
  /* 	 locn[4][Times-1][0], locn[4][Times-1][1], \ */
  /* 	 locn[5][Times-1][0], locn[5][Times-1][1], \ */
  /* 	 locn[6][Times-1][0], locn[6][Times-1][1] ); */
}

void printSuccessAll()
{
  printf("## Number of runs the base of the lineage was in a space (* = extant distrib)\n\n");
  // printf("lin  bor   wsu   ngu   phi   luz\n");
  printf("  Space       |  ");
  for (int k = 0; k < Spaces; k++) printf(" %3d ", k);
  printf("\n");
  printf("  ------------+--");
  for (int k = 0; k < Spaces; k++) printf("-----");
  printf("\n");
      
  for (int i = 0; i < Lineages; i++) {
    printf("  Lineage %3d |  ", i);
    for (int k = 0; k < Spaces; k++) {
      if (LineageExtant[i][Times-1][k])
        printf(" %3d*", record[i][k]);
      else  printf(" %3d ", record[i][k]);
    }
    printf("\n");
  }
  printf("\n");
    


  
}

double pDisp(int t, int a, int b)
{
  //! \todo Rename probDispA to probDisp and  probDispB to shapeDispA and shapeDisp to shapeDispB
  // The prob of dispersal between locations, per unit time, is
  //   P_DISP * 10^(area * modifier)
  double d = Cfg.probDispA * powf( Cfg.probDispB, -1.0 * Cfg.shapeDisp * 
            ( Dist[t][a][b] / maxdist));
  // if (Cfg.verbose==1) printf("d = %f\n", d);
  // return (double) expf(S_DISP * dist[time][a][b] ) * P_DISP;
  // printf("%f:  %f\n", dist[t][a][b], d);
  return d;
}

double pSurv(int t, int a)
{
  // The prob of surviving in a location, per unit time, is
  //   P_SURV * 0.1 * 10^(area * modifier)
  // printf("==%f==\n", powf((double) 10, ( (double) area[t][a] / (double) maxarea)) * 0.1 * (double) P_SURV);

  // s = 0.7;
  double s = (log10f( ( ( Area[t][a] / maxarea) * 99999 )+1) / 5) * Cfg.probSurv;
  // s = (log10f( ( ( (double) area[t][a] / (double) maxarea) * 99 )+1) / 2) * (double) survp;
  // s = powf((double) 10, ( (double) area[t][a] / (double) maxarea)) * 0.1 * (double) P_SURV;
  // if (Cfg.verbose==1) printf("s = %f\n", s);
  // printf("s = %f\n", s);
  return s;
}


/* int printMembers() */
/* { */
/*   int i, j; */
/*   for (i=0;i < lineageCnt; i++) */
/*     { */
/*       // for terminals */
/*       if (daughters[i][0] == 0) */
/* 		{ */
/* 		  printf("%d_%d", topresent, i); */
/* 		  for (j = 0; j < Spaces; j++) */
/* 			{ */
/* 			  printf(" %d", locn[i][Times-1][j]); */
/* 			} */
/* 		  printf("\n"); */
/* 		} */
/*     } */
/*   return 1; */
/* } */



