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
  occrunning = 0.0;
  maxarea = findMaxArea();
  maxdist = findMaxDist();

  printf("Maxarea: %f\n", maxarea);
  printf("Maxdist: %f\n", maxdist);

  printf("SETTINGS\n");
  printf("| surv = %5.3f |\n", Cfg.probSurv);
  obsocc = obsOccurrence();
  printf("| occ  = %5.3f |\n",obsocc);
  printf("+--------------+\n");

  // initialize srandom with time
  srandom(time(NULL));

  // clear counter variables
  long int r = 0; // runs

  // keep going until...
  while ((success < Cfg.stopAfterSuccess) && (r < Cfg.maxRuns))
    {
      
      // Run the main loop for X start areas
      biogeo();
      r++;

      // Reporting and adjustment every 1000 runs:
      if (!(r % 1000))
		{
		  // adjust base
		  // if ((occrunning / (double) topres1000) > obsocc) dispp2 += 0.05;
		  // if ((occrunning / (double) topres1000) < obsocc) dispp2 -= 0.05;

		  // adjust modifier
		  // 2009-09-13 drop:
		  //   if ((occrunning / (double) topres1000) > obsocc) dispp -= 0.05;
		  //   if ((occrunning / (double) topres1000) < obsocc) dispp += 0.05;
		  //   if (dispp > 1.0) dispp = 1.0;
		  //   if (dispp < 0.0) dispp = 0.0;

		  // Adjust surv
		  //if (topres1000 > lasttp1000) survp += 0.05;
		  //if ((occrunning / (double) topres1000) < obsocc) dispp2 -= 0.05;

		  fprintf(stderr, 
             "run %8ld; to present %8ld; success %8ld; occ %5.3f; disp %5.2f", 
				  r, (topresent- (topresent % 1000)), success, 
				  occrunning / (double) topres1000, Cfg.probDispA) ;
		  fflush(NULL);  // Flush the buffer
          
		  occrunning = 0.0;
		  topres1000 = 0;
          
		}
    }

  // Final reporting:
  fprintf(stderr, "\n"); fflush(NULL);
  printf("success/failure = %ld/%ld\n",          \
		 success, r - success);
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

  int l, s, t;
  int i, rnd;
  int places, linck, diffs;
  int occcnt;
  double tmpocccnt;

  // clear
  for (l = 0; l < Lineages; l++) {
    for (t = 0; t < Times; t++) {
      for (s = 0; s < Spaces; s++) {
		locn[l][t][s] = 0;
      } } }

  // establish starting point
  i = 0;
  while (i < Cfg.nStartSpaces)
    {
      rnd = (int) ((double) Spaces * (double) random() / (double) (RAND_MAX+1.0));
	  printf("%d\n", rnd);
      if ((locn[0][0][rnd] == 0) && \
		  (Area[0][rnd] != 0) && \
		  (Cfg.startSpace[rnd] == 1))
		{
		  locn[0][0][rnd] = 1;
		  i++;
		}
    }
  printf("%d %d %d %d\n", locn[0][0][0],locn[0][0][1],  locn[0][0][2], locn[0][0][3]);

  
  for ( t = 0; t < Times-1; t++)  // from T=0 to the penultimate period
    {
      // print array at start
      // if (Cfg.verbose) printArray(t);
 
      // check for each lineage in spacetime
      for (l = 0; l < Lineages; l++)
		{
		  linck = 0;
		  if (LineagePeriod[l][t]) // reduce the lineage set to examine to
			// just those lineages that are alive at t
			{
			  for (s = 0; s < Spaces; s++)
				{

				  // test for fossils: if there is a fossil in a place and
				  // no lineage in that place, this causes a failure
				  // if ((fossil[l][t][s] == 1) && (locn[l][t][s] == 0))
				  //   {
				  //     // printf("fossil failure\n");
				  //     return;
				  //   }

				  // where there is a lineage in a space at a time...
				  if (locn[l][t][s] == 1)
					{

					  // dispersal?
					  for (i = 0; i < Spaces ; i++)
						{
						  if ( ( ((double) random() / (double)(RAND_MAX+1.0)) \
								 < pDisp(t,s,i) ) && (i != s) && (Area[t][i] != 0))
							{
							  locn[l][t+1][i] = 1;
							  if (Cfg.verbose==1) printf("                                L%d DISPERSES FROM S%d TO S%d\n",l,s,i);
							  linck++;
							}
						}
		      
					  // survival in place  - Add size dept surv here
					  if ( ((double) random() / (double)(RAND_MAX+1.0)) \
						   < pSurv(t,s))
						{
						  locn[l][t+1][s] = 1;
						  linck++;
						}
					  else if (Cfg.verbose==1) printf( \
												  "                                L%d GOES LOCALLY EXTINCT IN S%d\n",l,s);

					}
				}
			  // check if this lineage (or daughter lineages) survive
			  if (linck == 0)
				{
				  if (Cfg.verbose==1) printf("                                L%d HAS DIED OUT\n",l);
				  if (Cfg.verbose==1) printf("\n-------------------------------------------------\n\n");
				  return;
				}
	    

			  // is it about to split?
			  if (LineagePeriod[l][t+1] == 0)
				{
				  // reallocate occurences to new taxa
				  // is the future lineage in one place? sympatric:
				  places = 0;
				  for (s = 0; s < Spaces; s++)
					{
					  places += locn[l][t+1][s];
					}
				  if (places == 1)
					{
					  if (Cfg.verbose==1) printf("                                SYMPATRIC SPECIATION\n");
					  for (s = 0; s < Spaces; s++)
						{
						  if (locn[l][t+1][s] == 1)
							{
							  // kill it:
							  locn[l][t+1][s] = 0;
							  // make daughters
							  locn[daughters[l][0]][t+1][s] = 1;
							  locn[daughters[l][1]][t+1][s] = 1;
							}
						}
					}
				  // or allopatric:
				  // Very simple - splits as in vicariant behaviour
				  else
					{
					  if (Cfg.verbose==1) printf("                                ALLOPATRIC SPECIATION\n");
					  // for (s = 0; s < Spaces; s++) printf("%d", locn[l][t+1][s]);
					  // printf("\n");
					  // pick one to be one species
					  i = -1;
					  while (i == -1)
						{
						  rnd = (int) ( (double) Spaces * (double) random() / (double) (RAND_MAX+1.0));
						  // printf("%d ", rnd);
						  if (locn[l][t+1][rnd] == 1) i = rnd;
						}
		      
					  for (s = 0; s < Spaces; s++)
						{
						  if (locn[l][t+1][s] == 1)
							{
							  // kill
							  locn[l][t+1][s] = 0;

							  //switch
							  if (s == i) locn[daughters[l][0]][t+1][s] = 1;
							  else locn[daughters[l][1]][t+1][s] = 1;
							}
						}
					}
				}
			}
		}
    }

  // All lineages made it to the present
  topresent++;
  topres1000++;

  // final layout
  /* if (Cfg.verbose==1)
    {
      printArray(Times-1);
      printExtant(Lineages);
      } */

  // check for success
  diffs = 0;
  tmpocccnt = 0.0;
  for (l = 0; l < Lineages; l++)
    {
      if (LineagePeriod[l][Times-1])
		{
		  occcnt = 0;
		  for (s = 0; s < Spaces; s++)
			{
			  if (locn[l][Times-1][s] != LineageExtant[l][Times-1][s]) diffs++;
			  occcnt += locn[l][Times-1][s];
			}
		  tmpocccnt += (double) occcnt / (double) Spaces;
		  //  printf("run %f; LinExtantN %d; occcnt %d; tmpocccnt %f\n", occrunning, LinExtantN, occcnt, tmpocccnt);
		}
    }

  occrunning += tmpocccnt / (double) LinExtantN;

  // An exactly matching present distribution; record positions of ancestral
  // nodes
  if (diffs == 0)
    {
      success++;

      for (l = 0; l < Lineages; l++)
		{
		  for (t = 0; t < Times; t++)
			{
			  if (LineagePeriod[l][t])
				{
				  for (s = 0; s < Spaces; s++)
					{
					  record[l][s] += locn[l][t][s];
					}
				  break;
				}
			}
		}
	  /*       if (Cfg.verbose==0)  */
	  /* 	{ */
	  /* 	  fprintf(stderr, "\r%3d",success); fflush(NULL); */
	  /* 	} */

      // if (Cfg.verbose==2) printSuccess();
    }

  if (Cfg.verbose==1) printf("\n-------------------------------------------------\n\n");
    
}

/* int printArray(int time) */
/* { */
/*   int i,x; */
/*   for (i = 0; i < Lineages; i++) */
/*     { */
/*       if (LineagePeriod[i][time] == 1) */
/* 		{ */
/* 		  printf("    T%2d L%2d  ", realtime[time], i); */
/* 		  for (x = 0; x < Spaces; x++) printf("%d", locn[i][time][x]); */
/* 		  printf("\n"); */
/* 		} */
/*     } */

/*   return 1; */
/* } */

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

/* int printSuccess() */
/* { */
/*   int i, j, k; */
/*   printf("int locn[Lineages][Times][Spaces] = { \\\n"); */
/*   for (i = 0; i < Lineages; i++) */
/*     { */
/*       printf("{ \\\n"); */
/*       for (j = 0; j < Times; j++) */
/* 		{ */
/* 		  printf("  {%d", locn[i][j][0]); */
/* 		  for (k = 1; k < Spaces; k++) */
/* 			{ */
/* 			  printf(", %d", locn[i][j][k]); */
/* 			} */
/* 		  if (j < Times-1) printf("}, \\\n"); */
/* 		  else printf("} "); */
/* 		} */
/*       if (i < Lineages-1) printf("}, \\\n"); */
/*       else printf("} \\\n"); */
/*     } */
/*   printf("};\n\n\n"); */

/*   /\*   printf("YES (%d%d %d%d %d%d) %d%d %d%d %d%d %d%d\n", \ *\/ */
/*   /\* 	 locn[0][1][0],  locn[0][1][1], \ *\/ */
/*   /\* 	 locn[1][6][0],  locn[1][6][1], \ *\/ */
/*   /\* 	 locn[2][9][0],  locn[2][9][1], \ *\/ */
/*   /\* 	 locn[3][Times-1][0], locn[3][Times-1][1], \ *\/ */
/*   /\* 	 locn[4][Times-1][0], locn[4][Times-1][1], \ *\/ */
/*   /\* 	 locn[5][Times-1][0], locn[5][Times-1][1], \ *\/ */
/*   /\* 	 locn[6][Times-1][0], locn[6][Times-1][1] ); *\/ */

/*   return 1; */
/* } */

int printSuccessAll()
{
  int i, k;
  printf("[N runs the base of each each lineage was in a place]:[final extant distrib]\n");
  printf("lin  bor   wsu   ngu   phi   luz\n");
  for (i = 0; i < Lineages; i++)
    {
      printf("%2d", i);
      for (k = 0; k < Spaces; k++)
		{
		  if (LineagePeriod[i][Times-1])
			{
			  printf(" %3d:%d", record[i][k], extant[i][k]);
			}
		  else  printf(" %3d: ", record[i][k]);
		}
      printf("\n");
    }

  
  return 1;
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



