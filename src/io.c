/*! \file
 * **Input, output, error and memory allocation functions**
 */

#define _GNU_SOURCE
#include <mxml.h>
#include "shiba.h"

/*! \brief Open file and parse XML. 
 *
 * Derived from mini-xml website examples. 
 */
void readXML()
{

  FILE *fp;
  mxml_node_t *tree;
  mxml_node_t *node;

  char **TimeLabel;  // The ID label in the XML file for the time period
  char **SpaceLabel; // The ID label in the XML file for the spaces
  char **PhyloLabel; // The ID label in the XML file for the phylogenies
  char **TaxonLabel; // The ID label in the XML file for the taxa

  // Counters for occurrences of data elements
  int t = 0; int s = 0; int p = 0; int x = 0; 
  int a = 0; int d = 0; int e = 0; int ss = 0;

  fp = fopen(DataFile, "r");
  tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
  fclose(fp);

  if (!tree) error("Unable to parse input file");


  // ----------- Get core dimensioning elements first using Xpath-ish -------- 

  // Note the quirk that mxmlFindPath returns the child not the requested node: 
  node = mxmlGetParent(mxmlFindPath(tree, "shibaInput/time")); 
  Times = atoi(mxmlElementGetAttr(node, "n"));
  if (!Times) error("time/@n not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "shibaInput/space")); 
  Spaces = atoi(mxmlElementGetAttr(node, "n"));
  if (!Spaces) error("space/@n not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "shibaInput/phylo")); 
  Phylos = atoi(mxmlElementGetAttr(node, "n"));
  if (!Phylos) error("phylo/@n not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "shibaInput/taxa")); 
  Taxa = atoi(mxmlElementGetAttr(node, "n"));
  if (!Taxa) error("taxa/@n not found in input");

  // --- single config options

  node = mxmlGetParent(mxmlFindPath(tree, "*/config/nStartSpaces")); 
  Cfg.nStartSpaces = atoi(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.nStartSpaces) error("//config/nStartSpaces not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "*/config/maxRuns")); 
  Cfg.maxRuns = atoi(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.maxRuns) error("//config/maxRuns not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "*/config/stopAfterSuccess")); 
  Cfg.stopAfterSuccess = atoi(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.stopAfterSuccess) error("//stopAfterSuccess not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "*/config/probSurv")); 
  Cfg.probSurv = atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.probSurv) error("//config/probSurv not found in input");
  
  node = mxmlGetParent(mxmlFindPath(tree, "*/config/probDispA")); 
  Cfg.probDispA = atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.probDispA) error("//config/probDispA not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "*/config/probDispB")); 
  Cfg.probDispB = atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.probDispB) error("//config/probDispB not found in input");

  node = mxmlGetParent(mxmlFindPath(tree, "*/config/shapeDisp")); 
  Cfg.shapeDisp = atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
  if (!Cfg.shapeDisp) error("//config/shapeDisp not found in input");

  // ------------- Dimension arrays in the heap -----------------------------

  Area       = mem2d_d(Times, Spaces);
  Dist       = mem3d_d(Times, Spaces, Spaces);
  RealTime   = mem1d_d(Times);
  TimeLabel  = mem2d1_c(Times);
  SpaceName  = mem2d1_c(Spaces);
  SpaceLabel = mem2d1_c(Spaces);
  Phylo      = mem2d1_c(Phylos);
  PhyloLabel = mem2d1_c(Phylos);
  Taxon      = mem2d1_c(Taxa);
  TaxonLabel = mem2d1_c(Taxa);
  Extant     = mem3d_i(Taxa, Times, Spaces);
  Cfg.startSpace = mem1d_i(Spaces);
  
  // --------------------- Crawl the XML tree to fill in the data --------- 

  node = tree;
  while(node)
    {
      if (mxmlGetElement(node))
        {
          // periodStart elements
          if (!strcmp(mxmlGetElement(node), "periodStart"))
            {
              if (t >= Times) error("Too many periodStart elements");
              if (!mxmlGetOpaque(mxmlGetFirstChild(node))) \
                error("A periodStart element is missing");
              else RealTime[t] = atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
              // test for declining times
              if (t > 0)
                {
                  /*! \todo Remove the constraint of having to have the ages
                   *  included in the XML file in decreasing order. Will need
                   *  a sorting stage
                   */
                  if (RealTime[t] > RealTime[t-1])
                    error("Time sequence is not sorted in declining age");
                }
              if (!mxmlElementGetAttr(node, "id"))            
                error("An id attr is missing in periodStart");
              else asprintf(&TimeLabel[t], "%s", 
                mxmlElementGetAttr(node, "id"));
              t++;
            }

          // spaceName elements
          else if (!strcmp(mxmlGetElement(node), "spaceName"))
            {
              if (s >= Spaces) error("Too many spaceName elements");
              if (!mxmlGetOpaque(mxmlGetFirstChild(node))) 
                error("A spaceName datum is missing");
              else asprintf(&SpaceName[s], "%s", 
                mxmlGetOpaque(mxmlGetFirstChild(node)));
              if (!mxmlElementGetAttr(node, "id"))            
                error("An id attr is missing in spaceName");
              else asprintf(&SpaceLabel[s], "%s", 
                mxmlElementGetAttr(node, "id"));
              s++;
            }

          // phylo elements
          else if (!strcmp(mxmlGetElement(node), "newick"))
            {
              if (p >= Phylos) error("Too many newick elements");
              if (!mxmlGetOpaque(mxmlGetFirstChild(node)))   
                error("A newick datum is missing");
              else 
                {
                  // scrub whitespace
                  char *tmp; int tmpn = 0;
                  asprintf(&tmp, "%s",
                           mxmlGetOpaque(mxmlGetFirstChild(node)));
                  for (int i = 0; i < strlen(tmp); i++)
                    if ((tmp[i] != 32) && 
                        (tmp[i] != 10) &&
                        (tmp[i] != 12) &&
                        (tmp[i] != 13)) {
                      if (!tmpn) {
                        asprintf(&Phylo[p], "%c", tmp[i]); tmpn++;
                      } else { 
                        Sasprintf(Phylo[p], "%s%c", Phylo[p], tmp[i]); tmpn++; 
                      }
                    }
                  free(tmp);
                }
              if (!mxmlElementGetAttr(node, "id"))          
                error("An id attr is missing in newick");
              else asprintf(&PhyloLabel[p], "%s", 
                mxmlElementGetAttr(node, "id"));
              p++;
            }

          // taxa elements
          else if (!strcmp(mxmlGetElement(node), "taxon"))
            {
              if (x >= Taxa) error("Too many taxon elements");
              if (!mxmlGetOpaque(mxmlGetFirstChild(node)))  \
                error("A taxon datum is missing");
              else asprintf(&Taxon[x], "%s",                \
                mxmlGetOpaque(mxmlGetFirstChild(node)));
              if (!mxmlElementGetAttr(node, "id"))          \
                error("An id attr is missing in taxon");
              else asprintf(&TaxonLabel[x], "%s", \
                mxmlElementGetAttr(node, "id"));
              x++;
            }

          // area elements
          else if (!strcmp(mxmlGetElement(node), "area"))
            {
              int a_t; int a_s;
              if (a >= Spaces * Times) error("Too many area elements");
              // Test for text child element
              if (!mxmlGetOpaque(mxmlGetFirstChild(node)))
                error("An area datum is missing");
              // Read the time attr
              if (!mxmlElementGetAttr(node, "time"))        \
                error("A time attr is missing in area");
              else
                {
                  a_t = -1;
                  for (int i = 0; i < Times; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "time"),
                        TimeLabel[i])) a_t = i;
                    }           
                  if (a_t == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //area/@time '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "time"));
                      exit(1);
                    }
                }
              // Read the space attr
              if (!mxmlElementGetAttr(node, "space")) {       
                error("A space attr is missing in area");
              } else {
                a_s = -1;
                for (int i = 0; i < Spaces; i++)
                  if (!strcmp(mxmlElementGetAttr(node, "space"),
                              SpaceLabel[i])) a_s = i;
                if (a_s == -1) {
                  fprintf(stderr, 
                    "XML parse error: //area/@space '%s' not IDREF\n", 
                     mxmlElementGetAttr(node, "space"));
                      exit(1);
                }
              }
              // Set the datum
              Area[a_t][a_s] = atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
              a++;
            }

          // dist elements
          else if (!strcmp(mxmlGetElement(node), "dist"))
            {
              int d_t; int d_s1; int d_s2;
              if (a > ((Spaces * (Spaces-1))/2) * Times) 
                error("Too many dist elements");
              // Test for text child element
              if (!mxmlGetOpaque(mxmlGetFirstChild(node)))
                error("An dist datum is missing");
              // Read the time attr
              if (!mxmlElementGetAttr(node, "time"))        \
                error("A time attr is missing in dist");
              else
                {
                  d_t = -1;
                  for (int i = 0; i < Times; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "time"),
                                  TimeLabel[i])) d_t = i;
                    }
                  if (d_t == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //dist/@time '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "time"));
                      exit(1);
                    }
                }
              // Read the space from attr
              if (!mxmlElementGetAttr(node, "from"))
                error("A from attr is missing in dist");
              else
                {
                  d_s1 = -1;
                  for (int i = 0; i < Spaces; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "from"),
                             SpaceLabel[i])) d_s1 = i;
                    }           
                  if (d_s1 == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //dist/@from '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "from"));
                      exit(1);
                    }
                }
              // Read the space to attr
              if (!mxmlElementGetAttr(node, "to"))
                error("A to attr is missing in dist");
              else
                {
                  d_s2 = -1;
                  for (int i = 0; i < Spaces; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "to"),
                             SpaceLabel[i])) d_s2 = i;
                    }           
                  if (d_s2 == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //dist/@to '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "to"));
                      exit(1);
                    }
                }
              // Set the datum (symetrically)
              Dist[d_t][d_s1][d_s2] = 
                atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
              Dist[d_t][d_s2][d_s1] = 
                atof(mxmlGetOpaque(mxmlGetFirstChild(node)));
              d++;
            }

          // extant elements
          else if (!strcmp(mxmlGetElement(node), "extant"))
            {
              int e_x; int e_t; int e_s;
              // NB, No Test for text child element

              // Read the taxon from attr
              if (!mxmlElementGetAttr(node, "taxon"))
                error("A taxon attr is missing in extant");
              else
                {
                  e_x = -1;
                  for (int i = 0; i < Taxa; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "taxon"),
                                  TaxonLabel[i])) e_x = i;
                    }           
                  if (e_x == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //extant/@taxon '%s' not IDREF\n", 
                              mxmlElementGetAttr(node, "taxon"));
                      exit(1);
                    }
                }
              // Read the time attr
              if (!mxmlElementGetAttr(node, "time"))        \
                error("A time attr is missing in extant");
              else
                {
                  e_t = -1;
                  for (int i = 0; i < Times; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "time"),
                                  TimeLabel[i])) e_t = i;
                    }
                  if (e_t == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //extant/@time '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "time"));
                      exit(1);
                    }
                }
              // Read the space to attr
              if (!mxmlElementGetAttr(node, "space"))
                error("A space attr is missing in extant");
              else
                {
                  e_s = -1;
                  for (int i = 0; i < Spaces; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "space"),
                                  SpaceLabel[i])) e_s = i;
                    }           
                  if (e_s == -1)
                    {
                      fprintf(stderr,
                        "XML parse error: //extant/@space '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "space"));
                      exit(1);
                    }
                }
              // Set the datum (symetrically)
              Extant[e_x][e_t][e_s] = 1;
              e++;
            }

          // ------------- Config --------------------------------------

          // startSpaces/allowed
          else if (!strcmp(mxmlGetElement(node), "allowed"))
            {
              int ss_s;
              if (ss >= Spaces) error("Too many startSpaces/allowed elements");
              // Read the attr
              if (!mxmlElementGetAttr(node, "space"))
                error("A space attr is missing in allowed");
              else
                {
                  ss_s = -1;
                  for (int i = 0; i < Spaces; i++)
                    {
                      if (!strcmp(mxmlElementGetAttr(node, "space"),
                                  SpaceLabel[i])) ss_s = i;
                    }           
                  if (ss_s == -1)
                    {
                      fprintf(stderr, 
                        "XML parse error: //allowed/@space '%s' not IDREF\n", 
                        mxmlElementGetAttr(node, "space"));
                      exit(1);
                    }
                }
              Cfg.startSpace[ss_s] = 1;
              ss++;
            }
        }
      
      node = mxmlWalkNext(node, tree, MXML_DESCEND);
      
    }

  // Check for correct dimensioning
  if (t != Times) error("Wrong number of periodStart elements");
  if (s != Spaces) error("Wrong number of spaceName elements");
  if (p != Phylos) error("Wrong number of newick elements");
  if (x != Taxa) error("Wrong number of taxon elements");
  if (a != (Spaces * Times)) error("Wrong number of area elements");
  if (d != ((Spaces * (Spaces-1))/2) * Times) 
    error("Wrong number of dist elements");
  if (e < 1) error("Need at least one extant taxon");


  // free unused mem
  mxmlDelete(tree);

  free2d1_c(PhyloLabel, Phylos);
  free2d1_c(SpaceLabel, Spaces);
  free2d1_c(TimeLabel, Times);
  free2d1_c(TaxonLabel, Taxa);

}

/*! \brief Output matrices for checking by user.
 */
void printIndata()
{
  printf("# Data as read from %s\n", DataFile);

  printf("\n## Spaces:\n");
  for (int i = 0; i < Spaces; i++) printf("  %3d: %s\n", i, SpaceName[i]); 

  printf("\n## Times:\n");
  for (int i = 0; i < Times; i++) printf("  %3d: %7.2f\n", i, RealTime[i]); 

  printf("\n## Areas:\n");
  printf("  Spaces  :");
  for (int i = 0; i < Spaces; i++) printf("  %6d", i); 
  printf("\n");
  for (int i = 0; i < Times; i++) 
    {
      printf("  Time %3d:", i);
      for (int j = 0; j < Spaces; j++) printf("  %6.2f", Area[i][j]);
      printf("\n");
    }

  printf("\n## Distances:\n");
  for (int t = 0; t < Times; t++)
    {
      printf("  Time: %3d\n", t);
      printf("    From    :");
      for (int i = 0; i < Spaces; i++) printf("  %6d", i); 
      printf("\n");
      for (int i = 0; i < Spaces; i++) 
        {
          printf("    To   %3d:", i);
          for (int j = 0; j < Spaces; j++) printf("  %6.1f", Dist[t][i][j]);
          printf("\n");
        }
    }

  printf("\n## Taxa:\n");
  for (int i = 0; i < Taxa; i++) printf("  %d: %s\n", i, Taxon[i]); 

  printf("\n## Phylos:\n");
  for (int i = 0; i < Phylos; i++) printf("  %d: %s\n", i, Phylo[i]); 

  printf("\n## LineagePeriods:\n");
  printf("  Lineage   :  ");
  for (int i = 0; i < Lineages; i++)
    {
      if ((i % 5) == 0) printf("|");
      else printf(" ");
    }
  printf("\n");
  for (int t = 0; t < Times; t++)
    {
      printf("  Period %3d:  ", t);
      for (int i = 0; i < Lineages; i++)
        {
          printf("%1d", LineagePeriod[i][t]);
        }
      printf("\n");
    }

  printf("\n## LineageDaughters:\n");
  for (int i = 0; i < Lineages; i++) {
    printf("  Lineage %3d:  ", i);
    //printf("  %3d", LineageNDaughters[i]);
    for (int j = 0; j < LineageNDaughters[i]; j++)
      printf("  %3d", LineageDaughters[i][j]);
    printf("\n");
  }

  printf("\n## ExtantTaxa:\n");
  printf("  Spaces             :");
  for (int i = 0; i < Spaces; i++) printf("  %3d", i); 
  printf("\n");
  for (int t = 0; t < Times; t++)
    {
      for (int i = 0; i < Taxa; i++)
        {
          int isOne = 0;
          for (int j = 0; j < Spaces; j++)
            if (Extant[i][t][j]) isOne++;
          if (isOne)
            {
              printf("  Time %3d, Taxon %3d:", t, i);
              for (int j = 0; j < Spaces; j++) printf("  %3d", Extant[i][t][j]);
              printf("\n");
            }
        }
    }

  printf("\n## LineageExtant:\n");
  printf("  Spaces               :");
  for (int i = 0; i < Spaces; i++) printf("  %3d", i); 
  printf("\n");
  for (int t = 0; t < Times; t++)
    {
      for (int i = 0; i < Lineages; i++)
        {
          int isOne = 0;
          for (int j = 0; j < Spaces; j++)
            if (LineageExtant[i][t][j]) isOne++;
          if (isOne)
            {
              printf("  Time %3d, Lineage %3d:", t, i);
              for (int j = 0; j < Spaces; j++) printf("  %3d", LineageExtant[i][t][j]);
              printf("\n");
            }
        }
    }

  printf("\n## StartSpaces:\n");
  printf("  Spaces  :");
  for (int i = 0; i < Spaces; i++) printf("  %3d", i); 
  printf("\n");
  printf("  Allowed :");
  for (int j = 0; j < Spaces; j++) printf("  %3d", Cfg.startSpace[j]);
  printf("\n");

  printf("\n## Config:\n");
  printf("  nStartSpaces     : %10d\n", Cfg.nStartSpaces);
  printf("  maxRuns          : %10d\n", Cfg.maxRuns);
  printf("  stopAfterSuccess : %10d\n", Cfg.stopAfterSuccess);
  printf("  probSurv         : %10.2f\n", Cfg.probSurv);
  printf("  probDispA        : %10.2f\n", Cfg.probDispA);
  printf("  probDispB        : %10.2f\n", Cfg.probDispB);
  printf("  shapeDisp        : %10.2f\n", Cfg.shapeDisp);

  printf("\n");

}

/*! \brief Basic error output and exit function.
 *
 * Note that no freeing is done, possibly leading to memory leaks.
 * @param error_msg the error message
 */

void error(char *error_msg)
{
  printf("ERROR:\n  %s\n", error_msg);
  exit(1);
  // NB: In the tcsh, `echo $status` returns the exit status of 
  // the preceding command.
}


/*! \brief Dimensions an initialized 1-D array of doubles.
 *
 * To free, just use free().
 * @param dimx The dimensions of the vector
 * @return A pointer to an array of doubles
 */

double* mem1d_d(int dimx)
{
  double *ptr;
  ptr = calloc(dimx, sizeof(double));
  if (!ptr) error("allocation failure in mem1d_d()");
  return ptr;
}

/*! \brief Dimensions an initialized 1-D array of int.
 *
 * To free, just use free().
 * @param dimx The dimensions of the vector
 * @return A pointer to an array of ints
 */

int* mem1d_i(int dimx)
{
  int *ptr;
  ptr = calloc(dimx, sizeof(int));
  if (!ptr) error("allocation failure in mem1d_i()");
  return ptr;
}

/*! Dimensions a initialized 1-D array of int pointers, ready to 
 * be allocated using mem1d_i() .
 * The result will be a `ragged' matrix. To free, use free2d1_i() .
 * @param dimx The dimensions of the vector
 * @return A pointer to an array of int pointers
 */

int** mem2d1_i(int dimx)
{
  int **ptr;
  ptr = calloc(dimx, sizeof(int *));
  if (!ptr) error("allocation failure in mem2d1_i()");
  return ptr;
}

/*! Frees the ragged array allocated by mem2d1_i().
 * @param dimx The dimensions of the vector
 */

void free2d1_i(int **ptr, int dimx)
{
  for(int i = 0; i < dimx; i++)
    free(ptr[i]);
  free(ptr);
}

/*! \brief Dimensions a initialized 1-D array of character pointers, ready to 
 * be allocated using asprintf.
 * 
 * The result will be a `ragged' matrix. To free, use free2d1_c() .
 * @param dimx The dimensions of the vector
 * @return A pointer to an array of character pointers
 */

char** mem2d1_c(int dimx)
{
  char **ptr;
  ptr = calloc(dimx, sizeof(char *));
  if (!ptr) error("allocation failure in mem2d1_c()");
  return ptr;

  // Direct, non-function alternative, e.g.: 
  // SpaceName = (char**) malloc( Spaces * sizeof(char*));
}

/*! \brief Frees the ragged array allocated by mem2d1_c().
 * @param dimx The dimensions of the vector
 */

void free2d1_c(char **ptr, int dimx)
{
  for(int i = 0; i < dimx; i++)
    free(ptr[i]);
  free(ptr);
}


/*!
 * Dimensions an initialized 2-D array of int. Free with free2d_i()
 */

int** mem2d_i(int dimx, int dimy)
{
  int **ptr;
  ptr = calloc(dimx, sizeof(int *));
  if (!ptr) error("allocation failure in mem2d_i() pass 1");
  for(int i = 0; i < dimx; i++) 
    {
      ptr[i] = calloc(dimy, sizeof(int));
      if (!ptr[i]) error("allocation failure in mem2d_i() pass 2");
    }
  return ptr;
}

void free2d_i(int **ptr, int dimx)
{
  for(int i = 0; i < dimx; i++)
    free(ptr[i]);
  free(ptr);
}

/*!
 * Dimensions an initialized 2-D array of doubles. Free with free2d_d()
 */

double** mem2d_d(int dimx, int dimy)
{
  double **ptr;
  ptr = calloc(dimx, sizeof(double *));
  if (!ptr) error("allocation failure in mem2d_d() pass 1");
  for(int i = 0; i < dimx; i++) 
    {
      ptr[i] = calloc(dimy, sizeof(double));
      if (!ptr[i]) error("allocation failure in mem2d_d() pass 2");
    }
  return ptr;
}

void free2d_d(double **ptr, int dimx)
{
  for(int i = 0; i < dimx; i++)
    free(ptr[i]);
  free(ptr);
}

/*!
 * Dimensions an initialized 3-D array of doubles. Free with free3d_d()
 */

double*** mem3d_d(int dimx, int dimy, int dimz)
{
  double ***ptr;
  ptr = calloc(dimx, sizeof(double **));
  if (!ptr) error("allocation failure in mem3d_d() pass 1");
  for(int i = 0; i < dimx; i++) 
    {
      ptr[i] = calloc(dimy, sizeof(double *));
      if (!ptr[i]) error("allocation failure in mem3d_d() pass 2");
      for(int j = 0; j < dimy; j++) 
        {
          ptr[i][j] = calloc(dimz, sizeof(double));
          if (!ptr[i][j]) error("allocation failure in mem3d_d() pass 3");
        }
    }
  return ptr;
}

void free3d_d(double ***ptr, int dimx, int dimy)
{
  for(int i = 0; i < dimx; i++)
    {
      for(int j = 0; j < dimy; j++)
        free(ptr[i][j]);
      free(ptr[i]);
    }
  free(ptr);
}

/*!
 * Dimensions an initialized 3-D array of int. Free with free3d_i()
 */

int*** mem3d_i(int dimx, int dimy, int dimz)
{
  int ***ptr;
  ptr = calloc(dimx, sizeof(int **));
  if (!ptr) error("allocation failure in mem3d_d() pass 1");
  for(int i = 0; i < dimx; i++) 
    {
      ptr[i] = calloc(dimy, sizeof(int *));
      if (!ptr[i]) error("allocation failure in mem3d_d() pass 2");
      for(int j = 0; j < dimy; j++) 
        {
          ptr[i][j] = calloc(dimz, sizeof(int));
          if (!ptr[i][j]) error("allocation failure in mem3d_d() pass 3");
        }
    }
  return ptr;
}

void free3d_i(int ***ptr, int dimx, int dimy)
{
  for(int i = 0; i < dimx; i++)
    {
      for(int j = 0; j < dimy; j++)
        free(ptr[i][j]);
      free(ptr[i]);
    }
  free(ptr);
}

//! Default help output.

void help()
{
  printf("\n  SHIBA (Simulated Historical Island Biogeography Analysis)\n");
  printf("  (c) Cam Webb 2013\n");
  printf("  Usage: shiba [ -h -f FILE -p INT ]\n");
  printf("  Options:\n");
  printf("     -f     Use this file as input. Default file: shibaInput.xml\n");
  printf("     -p     Use this phylogeny (0...n). Default: 0\n");
  printf("     -h     Print this help list\n\n");
  exit(0);
}

