// SHIBA: Stochastic Historical Biogeography Analysis  
// Cam Webb <cwebb@oeb.harvard.edu>
// $Revision: 1.9 $

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "shiba.h"

int main(int argc, char *argv[])
{
  // local counters:  r = Runs
  long int r, i, j;

  maxarea = findMaxArea();
  maxdist = findMaxDist();

  fprintf(stderr, "SHIBA\n");
  
  printf("SETTINGS\n");
  printf("| surv = %5.3f |\n", survp);

  ObsOcc();
  printf("| occ  = %5.3f |\n",obsocc);
  printf("+--------------+\n");
  // read the arguments
  if (argc == 1) 
    {
      printWelcome(); return(0);
    }
  else if (strcmp(argv[1], "-m") == 0) 
    {
      mmain();
	  exit(1);
    }
  else if (strcmp(argv[1], "-s") != 0) 
    {
      printf("Sorry!\n"); return(0);
    }

  // initialize srandom with time
  srandom(time(NULL));  

  // STEP

  // for (survp = 1.0; survp > 0.0; survp -= 0.1)
  //  {


  // clear counter variables
  r = 0;
  for (i = 0; i < LINEAGES; i++) {
    for ( j = 0; j < SPACE; j++) {
      record[i][j] = 0; } }

  success = 0; 

  // keep going until...
  while ((success < SUCCESSES) && (r < MAXRUNS))
    {

      // Run the main loop for X start areas
      Loop();
      r++;

      // Reporting:
      if ((r % 1000) == 0) 
		{
		  // adjust base
		  // if ((occrunning / (float) topres1000) > obsocc) dispp2 += 0.05;
		  // if ((occrunning / (float) topres1000) < obsocc) dispp2 -= 0.05;

		  // adjust modifier
		  // 2009-09-13 drop:
		  //   if ((occrunning / (float) topres1000) > obsocc) dispp -= 0.05;
		  //   if ((occrunning / (float) topres1000) < obsocc) dispp += 0.05;
		  //   if (dispp > 1.0) dispp = 1.0;
		  //   if (dispp < 0.0) dispp = 0.0;


		  // Adjust surv
		  //if (topres1000 > lasttp1000) survp += 0.05;
		  //if ((occrunning / (float) topres1000) < obsocc) dispp2 -= 0.05;

		  fprintf(stderr, "run %8d; to present %8d; success %4d; occ %5.3f; disp %5.2f",\
				  (int) r, (topresent- (topresent % 1000)), success, \
				  occrunning / (float) topres1000, dispp) ; 
		  fflush(NULL);  // Flush the buffer

		  occrunning = 0.0;
		  topres1000 = 0;
		}
    }

  // Final reporting:
  fprintf(stderr, "\n"); fflush(NULL);
  printf("success/failure = %d/%ld\n", \
		 success, r - (long) success);
  printSuccessAll();

  //  }

  return 1;
}

void Loop()
{
  int l, s, t;
  int i, rnd;
  int places, linck, diffs;
  int occcnt;
  float tmpocccnt;

  // clear
  for (l = 0; l < LINEAGES; l++) {
    for (t = 0; t < TIME; t++) {
      for (s = 0; s < SPACE; s++) {
		locn[l][t][s] = 0;
      } } }

  // establish starting point
  i = 0;
  while (i < STARTAREAS)
    {
      rnd = (int) ((float) SPACE * (float) random() / (float) (RAND_MAX+1.0));
	  printf("%d\n", rnd);
      if ((locn[0][0][rnd] == 0) && \
		  (area[0][rnd] != 0) && \
		  (constrains[rnd] == 1))  
		{
		  locn[0][0][rnd] = 1; 
		  i++;
		}
    }
  printf("%d %d %d %d\n", locn[0][0][0],locn[0][0][1],  locn[0][0][2], locn[0][0][3]);

  
  for ( t = 0; t < TIME-1; t++)  // from T=0 to the penultimate period
    {
      // print array at start
      if (VERBOSE) printArray(t);
 
      // check for each lineage in spacetime
      for (l = 0; l < LINEAGES; l++) 
		{
		  linck = 0;
		  if (live[l][t]) // reduce the lineage set to examine to
			// just those lineages that are alive at t
			{
			  for (s = 0; s < SPACE; s++)
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
					  for (i = 0; i < SPACE ; i++)
						{
						  if ( ( ((float) random() / (float)(RAND_MAX+1.0)) \
								 < pDisp(t,s,i) ) && (i != s) && (area[t][i] != 0) \
							   && (constraind[i]==1)) 
							{
							  locn[l][t+1][i] = 1;
							  if (VERBOSE==1) printf("                                L%d DISPERSES FROM S%d TO S%d\n",l,s,i);
							  linck++;
							}
						}
		      
					  // survival in place  - Add size dept surv here
					  if ( ((float) random() / (float)(RAND_MAX+1.0)) \
						   < pSurv(t,s))
						{
						  locn[l][t+1][s] = 1;
						  linck++;
						}
					  else if (VERBOSE==1) printf( \
												  "                                L%d GOES LOCALLY EXTINCT IN S%d\n",l,s);

					}
				}
			  // check if this lineage (or daughter lineages) survive
			  if (linck == 0)
				{
				  if (VERBOSE==1) printf("                                L%d HAS DIED OUT\n",l);
				  if (VERBOSE==1) printf("\n-------------------------------------------------\n\n");
				  return;
				}
	    

			  // is it about to split?
			  if (live[l][t+1] == 0)
				{
				  // reallocate occurences to new taxa
				  // is the future lineage in one place? sympatric:
				  places = 0;
				  for (s = 0; s < SPACE; s++)
					{
					  places += locn[l][t+1][s];
					}
				  if (places == 1)
					{
					  if (VERBOSE==1) printf("                                SYMPATRIC SPECIATION\n");
					  for (s = 0; s < SPACE; s++)
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
					  if (VERBOSE==1) printf("                                ALLOPATRIC SPECIATION\n");
					  // for (s = 0; s < SPACE; s++) printf("%d", locn[l][t+1][s]);
					  // printf("\n");
					  // pick one to be one species
					  i = -1;
					  while (i == -1)
						{
						  rnd = (int) ( (float) SPACE * (float) random() / (float) (RAND_MAX+1.0));
						  // printf("%d ", rnd);
						  if (locn[l][t+1][rnd] == 1) i = rnd;
						}
		      
					  for (s = 0; s < SPACE; s++)
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
  if (VERBOSE==1) 
    {
      printArray(TIME-1);
      printExtant(LINEAGES);
    }

  // check for success
  diffs = 0;
  tmpocccnt = 0.0;
  for (l = 0; l < LINEAGES; l++)
    {
      if (live[l][TIME-1])
		{
		  occcnt = 0;
		  for (s = 0; s < SPACE; s++)
			{
			  if (locn[l][TIME-1][s] != extant[l][s]) diffs++;
			  occcnt += locn[l][TIME-1][s];
			}
		  tmpocccnt += (float) occcnt / (float) SPACE;
		  //  printf("run %f; extntlin %d; occcnt %d; tmpocccnt %f\n", occrunning, extntlin, occcnt, tmpocccnt);
		}
    }

  occrunning += tmpocccnt / (float) extntlin;

  // An exactly matching present distribution; record positions of ancestral
  // nodes 
  if (diffs == 0)
    {
      success++;

      for (l = 0; l < LINEAGES; l++) 
		{
		  for (t = 0; t < TIME; t++) 
			{
			  if (live[l][t])
				{
				  for (s = 0; s < SPACE; s++) 
					{
					  record[l][s] += locn[l][t][s];
					}
				  break;
				}
			}
		}
	  /*       if (VERBOSE==0)  */
	  /* 	{ */
	  /* 	  fprintf(stderr, "\r%3d",success); fflush(NULL); */
	  /* 	} */

      if (VERBOSE==2) printSuccess();
    }

  if (VERBOSE==1) printf("\n-------------------------------------------------\n\n");
    
  return;

}

int printArray(int time)
{
  int i,x;
  for (i = 0; i < LINEAGES; i++)
    {
      if (live[i][time] == 1)
		{
		  printf("    T%2d L%2d  ", realtime[time], i);
		  for (x = 0; x < SPACE; x++) printf("%d", locn[i][time][x]);
		  printf("\n");
		}
    }

  return 1;
}

int printExtant(int lins)
{
  int i,x;
  printf("CUR\n");
  for (i = 0; i < lins; i++)
    {
      if (live[i][TIME-1] == 1)
		{
		  printf("    T%2d L%2d  ", realtime[TIME-1], i);
		  for (x = 0; x < SPACE; x++) printf("%d", extant[i][x]);
		  printf("\n");
		}
    }

  return 1;
}

int printSuccess()
{
  int i, j, k;
  printf("int locn[LINEAGES][TIME][SPACE] = { \\\n");
  for (i = 0; i < LINEAGES; i++)
    {
      printf("{ \\\n");
      for (j = 0; j < TIME; j++)
		{
		  printf("  {%d", locn[i][j][0]);
		  for (k = 1; k < SPACE; k++)
			{
			  printf(", %d", locn[i][j][k]);
			}
		  if (j < TIME-1) printf("}, \\\n");
		  else printf("} ");
		}
      if (i < LINEAGES-1) printf("}, \\\n");
      else printf("} \\\n");
    }
  printf("};\n\n\n");

  /*   printf("YES (%d%d %d%d %d%d) %d%d %d%d %d%d %d%d\n", \ */
  /* 	 locn[0][1][0],  locn[0][1][1], \ */
  /* 	 locn[1][6][0],  locn[1][6][1], \ */
  /* 	 locn[2][9][0],  locn[2][9][1], \ */
  /* 	 locn[3][TIME-1][0], locn[3][TIME-1][1], \ */
  /* 	 locn[4][TIME-1][0], locn[4][TIME-1][1], \ */
  /* 	 locn[5][TIME-1][0], locn[5][TIME-1][1], \ */
  /* 	 locn[6][TIME-1][0], locn[6][TIME-1][1] ); */

  return 1;
}
int printSuccessAll()
{
  int i, k;
  printf("[N runs the base of each each lineage was in a place]:[final extant distrib]\n");
  printf("lin  bor   wsu   ngu   phi   luz\n");
  for (i = 0; i < LINEAGES; i++)
    {
      printf("%2d", i);
      for (k = 0; k < SPACE; k++)
		{
		  if (live[i][TIME-1])
			{
			  printf(" %3d:%d", record[i][k], extant[i][k]);
			}
		  else  printf(" %3d: ", record[i][k]); 
		}
      printf("\n");
    }

  
  return 1;
}

float pDisp(int t, int a, int b)
{
  float d;
  // The prob of dispersal between locations, per unit time, is 
  //   P_DISP * 10^(area * modifier)
  d = powf((float) dispp2, -1.0 * S_DISP *( (float) dist[t][a][b] / \
											(float) maxdist)) * dispp;
  // if (VERBOSE==1) printf("d = %f\n", d);
  // return (float) expf(S_DISP * dist[time][a][b] ) * P_DISP;
  // printf("%f:  %f\n", dist[t][a][b], d);
  return d;
}

/* float pDispGenerate() */
/* { */
/*   int a, b, t; */
/*   for (t = 0; t < TIME; t++) */
/*     { */
/*       for (a = 0; a < SPACE; a++) */
/* 	{ */
/* 	  for (b = 0; b < SPACE; b++) */
/* 	    { */
/* 	      pdisp[t][a][b] = powf( (float) 10, (-1.0 * S_DISP * \ */
/* 				        ( (float) dist[t][a][b] / \ */
/* 					  (float) maxdist) ) )    \ */
/* 			                * (float) P_DISP; */
/* 	    } */
/* 	} */
/*     } */
/* } */

float pSurv(int t, int a)
{
  float s;
  // The prob of surviving in a location, per unit time, is 
  //   P_SURV * 0.1 * 10^(area * modifier)
  // printf("==%f==\n", powf((float) 10, ( (float) area[t][a] / (float) maxarea)) * 0.1 * (float) P_SURV);

  // s = 0.7;
  s = (log10f( ( ( (float) area[t][a] / (float) maxarea) * 99999 )+1) / 5) * (float) survp;  
  // s = (log10f( ( ( (float) area[t][a] / (float) maxarea) * 99 )+1) / 2) * (float) survp;  
  // s = powf((float) 10, ( (float) area[t][a] / (float) maxarea)) * 0.1 * (float) P_SURV;
  // if (VERBOSE==1) printf("s = %f\n", s);
  // printf("s = %f\n", s);
  return s;
}

int findMaxArea()
{
  // finds total area on the landscape
  // was - max island size.  Now unlikely that any area get 100% survival
  int t, a, tot, max = 0;
  for (t = 0; t < TIME; t++) 
    {
      tot = 0;
      for (a = 0; a < SPACE; a++) 
		{
		  tot += area[t][a];
		}
      if (tot > max) max = tot; 
    }
  return max;
}

int findMaxDist()
{
  int t, a, b, max = 0;
  for (t = 0; t < TIME; t++) {
    for (a = 0; a < SPACE; a++) {
      for (b = 0; b < SPACE; b++) {
		if (dist[t][a][b] > max) max = dist[t][a][b];
      }
    }
  }
  return max;
}


void printWelcome()
{
  printf("This is SHIBA\n");
}


/* int mmain() */
/* { */
/*   long r; */
/*   int i; */

/*   srandom(time(NULL));   */

/*   success = 0; */
/*   r = 0; */

/*   for (i = 0; i < SPACE; i++) ending[i]=0; */

/*   if(VERBOSE==3) printf("\nsp bor wsu esu vog hal ngu spk nng eph luz car\n"); */
/*   if(VERBOSE==4) printf("\nsp bor wsu esu vog hal ngu spk nng eph luz car\n"); */

/*   while ((success < SUCCESSES) && (r < MAXRUNS)) */
/*     { */
/*       mLoop(1); */
/*       r++; */
/*     } */

/*   if (VERBOSE != 3) */
/*     { */
/*       printf("success/failure = %d/%ld\nSTR", \ */
/* 	     success, r - (long) success); */
/*       for (i = 0; i < SPACE; i++) printf(" %3d",starting[i]); */
/*       printf("\n    bor wsu esu vog hal ngu spk nng eph luz car\nEND"); */
/*       for (i = 0; i < SPACE; i++) printf(" %3d",ending[i]); */
/*       printf("\n"); */
/*     } */
/*   return 1; */
/* } */

void mLoop()
{
  int l, s, t;
  int i, rnd;
  int places, linck;

  // clear
  for (l = 0; l < LINEAGES; l++) {
	//            was MAXLINEAGES check

    daughters[l][0] = 0; daughters[l][1] = 0;

    for (t = 0; t < TIME; t++) {

      live[l][t] = 0;

      for (s = 0; s < SPACE; s++) {
		locn[l][t][s] = 0;
		extant[l][s] = 0; // wasted initialization
		bpa[l][s]=0;
		starting[s] = 0;
		end[s] = 0;
      } } }

  // start lineage

  live[0][0] = 1;
  lineageCnt = 1;

  // establish starting point
  i = 0;
  if (RNDSTART)
    {
      while (i < STARTAREAS)
		{
		  rnd = (int) ((float) SPACE * (float) random() / (float) (RAND_MAX+1.0));
		  if ((locn[0][0][rnd] == 0) && (area[0][rnd] != 0)) {
			locn[0][0][rnd] = 1; i++; starting[rnd] = 1;}
		}
    }
  else
    {
      locn[0][0][START_IN] = 1;
      starting[START_IN] = 1;
    }
  // printf("%d %d %d %d\n", locn[0][0][0],locn[0][0][1],  locn[0][0][2], locn[0][0][3]);

  for ( t = 0; t < TIME-1; t++)  // from T=0 to the penultimate period
    {
      // print array at start
      if (VERBOSE==1) printArray(t);
 
      // check for each lineage in spacetime
      for (l = 0; (l < lineageCnt) && (lineageCnt <= LINEAGES); l++)
		//                                           was MAXLINEAGES 
		{
		  linck = 0;
		  if (live[l][t] == 1) // reduce the lineage set to examine to
			// just those lineageCnt that are alive at t
			{
			  for (s = 0; s < SPACE; s++)
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
					  for (i = 0; i < SPACE ; i++)
						{
						  if ( ( ((float) random() / (float)(RAND_MAX+1.0)) \
								 < pDisp(t,s,i) ) && (i != s) && (area[t][i] != 0)) 
							{
							  locn[l][t+1][i] = 1;
							  if (VERBOSE==1) printf("                                L%d DISPERSES FROM S%d TO S%d\n",l,s,i);
							  linck++;
							  live[l][t+1] = 1; // it survived
							}
						}
		      
					  // survival in place
					  if ( ((float) random() / (float)(RAND_MAX+1.0)) \
						   < pSurv(t,s)) 
						{
						  locn[l][t+1][s] = 1;
						  linck++;
						  live[l][t+1] = 1; // it survived
						}
					  else if (VERBOSE==1) printf("                                L%d GOES LOCALLY EXTINCT IN S%d\n",l,s);

					}
				}
			  // check if this lineage (or daughter lineageCnt) survive
			  if (linck == 0)
				{
				  live[l][t+1] = 0;
				  if (VERBOSE==1) printf("                                L%d HAS DIED OUT\n",l);
				  if (VERBOSE==1) printf("\n-------------------------------------------------\n\n");
				  return; // in this simple model, no pruned phylos allowed - all must survive
				}
	    

			  // is it about to split?
			  if ( ((float) random() / (float)(RAND_MAX+1.0)) \
				   < pSpeciate) 
				{
				  // bookkeeping

				  live[l][t+1] = 0; 
				  daughters[l][0] = lineageCnt+0;
				  daughters[l][1] = lineageCnt+1;
				  live[lineageCnt+0][t+1] = 1;
				  live[lineageCnt+1][t+1] = 1;
				  lineageCnt +=2;
		      
				  // reallocate occurences to new taxa
				  // is the future lineage in one place? sympatric:

				  places = 0;
				  for (s = 0; s < SPACE; s++)
					{
					  places += locn[l][t+1][s];
					}
				  if (places == 1)
					{
					  if (VERBOSE==1) printf("                                SYMPATRIC SPECIATION\n");
		      
					  for (s = 0; s < SPACE; s++)
						{
						  if (locn[l][t+1][s] == 1)
							{
							  // kill it:  locn still records old l position
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
					  if (VERBOSE==1) printf("                                ALLOPATRIC SPECIATION\n");
					  // for (s = 0; s < SPACE; s++) printf("%d", locn[l][t+1][s]);
					  // printf("\n");
					  // pick one to be one species
					  i = -1;
					  while (i == -1)
						{
						  rnd = (int) ( (float) SPACE * (float) random() / (float) (RAND_MAX+1.0));
						  // printf("%d ", rnd);
						  if (locn[l][t+1][rnd] == 1) i = rnd;
						}
		      
					  for (s = 0; s < SPACE; s++)
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

  // final layout
  if (VERBOSE==1) 
    {
      printArray(TIME-1);
      // printExtant(LineageCnt);
    }

  if (lineageCnt == LINEAGES)
    {
      topresent++;

	  // create extant
      for (l = 0; l < lineageCnt; l++)
		{
		  for (s = 0; s < SPACE; s++)
			{
			  if (locn[l][TIME-1][s] == 1) 
				{
				  extant[l][s] = 1;
				  ending[s]++;
				  end[s]++;
				}
			}
		}

      if (VERBOSE==2) mprintSuccess();
      if (VERBOSE==3) printMembers();
	  printMembers();
      if (VERBOSE==4) {
		// mprintSuccess();
		outputBPA();
      }
    }

  if (VERBOSE==1) printf("\n-------------------------------------------------\n\n");
    
  return;

}

int mprintSuccess()
{
  int i, j;
  // printf("SUCCESS!\n");
  /*   printf("YES (%d%d %d%d %d%d) %d%d %d%d %d%d %d%d\n", \ */
  /* 	 locn[0][1][0],  locn[0][1][1], \ */
  /* 	 locn[1][6][0],  locn[1][6][1], \ */
  /* 	 locn[2][9][0],  locn[2][9][1], \ */
  /* 	 locn[3][TIME-1][0], locn[3][TIME-1][1], \ */
  /* 	 locn[4][TIME-1][0], locn[4][TIME-1][1], \ */
  /* 	 locn[5][TIME-1][0], locn[5][TIME-1][1], \ */
  /* 	 locn[6][TIME-1][0], locn[6][TIME-1][1] ); */

  printf("int starting[SPACE] = {%d", starting[0]);
  for (i = 1; i < SPACE; i++)
    {
      printf(", %d", starting[i]);
    }
  printf("};\n");

  printf("int live[LINEAGES][TIME] = { \\\n");
  for (i = 0; i < lineageCnt; i++)
    {
      printf("  {%d", live[i][0]);
      for (j = 1; j < TIME; j++)
		{
		  printf(", %d", live[i][j]);
		}
      if (i < lineageCnt-1) printf("}, \\\n");
      else printf("} \\\n");
    }
  printf("};\n");
  printf("int daughters[LINEAGES][2] = { \\\n");
  for (i = 0; i < lineageCnt; i++)
    {
      printf("  {%d, %d}", daughters[i][0], daughters[i][1]);
      if (i < lineageCnt-1) printf(", \\\n");
      else printf(" \\\n");
    }
  printf("};\n");

  printf("int extant[LINEAGES][SPACE] = { \\\n");
  for (i = 0; i < lineageCnt; i++)
    {
      printf("  {%d", extant[i][0]);
      for (j = 1; j < SPACE; j++)
		{
		  printf(", %d", extant[i][j]);
		}
      if (i < lineageCnt-1) printf("}, \\\n");
      else printf("} \\\n");
    }
  printf("};\n\n\n");

  return 1;
}

int printMembers()
{
  int i, j;
  for (i=0;i < lineageCnt; i++)
    {
      // for terminals
      if (daughters[i][0] == 0)
		{
		  printf("%d_%d", topresent, i);
		  for (j = 0; j < SPACE; j++)
			{
			  printf(" %d", locn[i][TIME-1][j]);
			}
		  printf("\n");
		}
    }
  return 1;
}

int outputBPA()
{
  int i, j;
  // get the tips first
  for (i = 0; i < lineageCnt; i++)
    {
      for (j = 0; j < SPACE; j++)
		{
		  if (locn[i][TIME-1][j]==1)
			{
			  bpa[i][j] = 1;
			}
		  else if (end[j] != 0)
			{
			  bpa[i][j] = 0;
			}
		  else bpa[i][j] = 9;
		}
    }
  // then work back up the tree
  for (i = lineageCnt-1; i >= 0; i--) // this works because the order of lineage index is exactlt the same order as the diversification 
    {
      for (j = 0; j < SPACE; j++)
		{
		  // for terminals
		  if (daughters[i][0] != 0)
			{
			  if (end[j] == 0) bpa[i][j] = 9;
			  else if ((bpa[daughters[i][0]][j]==1) || (bpa[daughters[i][1]][j]==1))
				{
				  bpa[i][j] = 1;
				}
			  else bpa[i][j] = 0;
			}
		}
    }
  // now print
  for (i=0;i < lineageCnt; i++)
    {
      printf("%d_%d", success, i);
      for (j = 0; j < SPACE; j++)
		{
		  printf(" %d", bpa[i][j]);
		}
      printf("\n");
    }
  return 1;
}


void ObsOcc()
{
  int i, j; 
  int cnt;
  float tmptot = 0.0;

  for (i = 0; i < LINEAGES; i++)
    {
      if (live[i][TIME-1])
		{      
		  cnt = 0;
		  for (j = 0; j < SPACE; j++)
			{
			  if (extant[i][j]) cnt++;
			}	
		  extntlin++; // the number of lineages at pres
		  tmptot += (float) cnt / (float) SPACE;
		  // printf("extntlin %d; cnt %d; tmptot %f\n", extntlin, cnt, tmptot);
		}
    }
  obsocc = (float) tmptot / (float) extntlin;
}

int mmain()
{
  long r;
  int i;

  srandom(time(NULL));  

  topresent = 0;
  r = 0;

  for (i = 0; i < SPACE; i++) ending[i]=0;

  if(VERBOSE==3) printf("\nsp bor wsu esu vog hal ngu spk nng eph luz car\n");
  if(VERBOSE==4) printf("\nsp bor wsu esu vog hal ngu spk nng eph luz car\n");

  while ((topresent < SUCCESSES) && (r < MAXRUNS))
    {
      mLoop(1);
      r++;
    }

  if (VERBOSE != 3)
    {
      printf("topresent/failure = %d/%ld\nSTR", \
			 topresent, r - (long) topresent);
      for (i = 0; i < SPACE; i++) printf(" %3d",starting[i]);
      printf("\n    bor wsu esu vog hal ngu spk nng eph luz car\nEND");
      for (i = 0; i < SPACE; i++) printf(" %3d",ending[i]);
      printf("\n");
    }
  return 1;
}
