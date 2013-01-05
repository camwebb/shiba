/*! \file
 * **Parses newick tree strings**
 */

#include "shiba.h"

phylo parseNewick(char *in) {

  phylo p;
  int lbrack = 0;
  int rbrack = 0;
  int comma = 0;

  // dimension the phylo struct
  for (int i = 0; i < strlen(in); i++) 
    {
      // printf("%c", in[i]);
      if      (in[i] == 40) lbrack++;
      else if (in[i] == 41) rbrack++;
      else if (in[i] == 44) comma++;
    }
  if (lbrack != rbrack) error("Imbalanced parentheses in phylogeny file");

  p.nnodes = lbrack + comma + 1; 
  p.parent = mem1d_i(p.nnodes);
  p.ndaughter = mem1d_i(p.nnodes);
  p.depth = mem1d_i(p.nnodes);
  p.bl = mem1d_d(p.nnodes);
  p.taxon = mem2d1_c(p.nnodes);
  p.age = mem1d_d(p.nnodes);

  // Move through the newick format tree character by character 
  int i = 0;
  // start outside parentheses, one hypothetical node proximal to root node
  int node = -1;
  int parent = -2;
  int nodeCount = 0;
  int tmpn;

  while ((i < strlen(in)) && (in[i] != 59))
    {
      // descend a branch, making new node for the parenthesis descended thru
      if (in[i] == 40) // "("
        {
          parent = node;
          // make a new node, set node to index of final row
          nodeCount++;
          node = nodeCount-1;
          p.parent[node] = parent;
          // first run case, since root has no parent data 
          if (node == 0) {
            p.depth[node] = 0;
          } else {
            p.depth[node] = p.depth[parent] + 1;
            p.ndaughter[parent]++;
          }
          /* printf("%c DOWN %d(%d)\n", in[i], node, parent);
             printf("  new node %d, parent %d, depth %d\n", 
             node, p.parent[node], p.depth[node]); */
          i++;
        }

      // sibling taxon, ignore
      else if (in[i] == 44) // ","
        {
          node = parent;
          parent = p.parent[node];
          // printf("%c ACROSS %d(%d)\n", in[i], node, parent);
          i++;
        }

      // back up a node to parent, keep track of locn with node
      else if (in[i] == 41) // ")"
        {
          node = parent;
          parent = p.parent[node];
          // printf("%c UP %d(%d)\n", in[i], node, parent);
          i++;
        }

      else
        {
          // Now, check for new taxa, or new interior name
         
          // Is there nodename here?  I.e., not `:' or `['
          if ((in[i] != 58) && (in[i] != 91))
            {
              // Is it a new terminal? I.e., not following a `)' 
              if (in[i-1] != 41)
                {
                  // creat new node
                  parent = node;
                  nodeCount++;
                  node = nodeCount-1;
                  p.parent[node] = parent;
                  p.depth[node] = p.depth[parent] + 1;
                  p.ndaughter[parent]++;
                  
                  /* printf("%c TERM %d(%d)\n", in[i], node, parent);
                  printf("  new node %d, parent %d, depth %d\n", node, 
                  p.parent[node], p.depth[node]); */
                }
          
              char *tmp = NULL; tmpn = 0;
              // check for name, I.e. not `:' nor `['
              while ((in[i] != 58) && (in[i] != 91) && 
                     (in[i] != 44) && (in[i] != 41) && (in[i] != 40) &&
                     (in[i] != 59))
                {
                  if (!tmpn) {
                    asprintf(&tmp, "%c", in[i]); tmpn++;
                  } else {
                    char *tmp2 = tmp;
                    asprintf(&tmp, "%s%c", tmp2, in[i]); tmpn++;
                    free(tmp2);
                  }
                  i++;
                }
              // was there any taxon name?
              asprintf(&p.taxon[node], "%s", tmp);
              // printf("  taxon name %s\n", tmp);
              free(tmp);
            }
          
          // are there bls?
          if (in[i] == 58)
            {
              i++; // skip over delimiter `:'
              char *tmp = NULL; tmpn = 0;
              // bead bl string, I.e. not `['
              while ((in[i] != 91) &&
                     (in[i] != 44) && (in[i] != 41) && (in[i] != 40) &&
                     (in[i] != 59)) // watch out for final `;'
                {
                  if (!tmpn) {
                    asprintf(&tmp, "%c", in[i]); tmpn++;
                  } else {
                    Sasprintf(tmp, "%s%c", tmp, in[i]); tmpn++;
                  }
                  i++;
                }
              p.bl[node] = atof(tmp);
              // printf("  taxon bl %f\n", p.bl[node]);
              free(tmp);
            }
          
          // are there notes?
          if (in[i] == 91)
            // Discard note
            while ((in[i] != 44) && (in[i] != 41) && (in[i] != 40) &&
                   (in[i] != 59))
              i++;
        }
    }

  /* printf("%s\n", in);
  for (int n = 0; n < p.nnodes; n++)
      printf("%3d %3d %3d %3d %5.2f %s\n", n, p.parent[n], p.depth[n], 
      p.ndaughter[n], p.bl[n], p.taxon[n]); */


  return p; 
}

/*!
 * **Reconciles the phylogeny edges with the geo time periods**
 *
 * For example, imagine three past time periods (A, B, C) and the future (Z),
 * and several phylogeny edges (a-k):
 *
 *      Z :   a     b   d     e   g     h    j     
 *      ..:...|.....|...|.....|...|.....|....|..... NOW
 *        :   |     |   |     |   +--+--+    |
 *      C :   +--+--+   |     |      |i      |
 *        :      |c     |     |      +----+--+
 *      ..:......|......|.....|...........|........
 *        :      |      +--+--+           |k
 *      B :      |         |f             |
 *      ..:......|.........|..............|........
 *      A :      |         |              | 
 *
 * These would be encoded with reference to the time periods as:
 *
 *               a b c    d e f    g h i j k
 *               -----    -----    ---------
 *           Z | 1 1 0    1 1 0    1 1 x 1 0
 *           C | 0 0 1    1 1 0    0 0 x 0 1
 *           B | 0 0 1    0 0 1    0 0 x 0 1
 *           A | 0 0 1    0 0 1    0 0 x 0 1
 *
 * Note that we need to push the branching events `forward,' so that
 * there is only ever one root stem passing through the beginning of
 * the first time period.
 */


void phyloToLineage(phylo p)
{

  // Check first for ultrametic tree
  double ageToRootStem = 0;
  double tmp;
  int node;

  Lineages = p.nnodes;
  for (int i = 0; i < p.nnodes; i++)
    if (!p.ndaughter[i]) {
      tmp = 0.0;
      node = i;
      while (p.parent[node] != -1) {
        p.age[node] = tmp;
        tmp += p.bl[node];
        node = p.parent[node];
      }
      if (!ageToRootStem) ageToRootStem = tmp;
      else if (tmp != ageToRootStem) error("tree is not ultrametric");
    }
  //add stem
  p.age[0] = ageToRootStem;
  ageToRootStem += p.bl[0];

  // Is the tree stem longer than the time slices?
  if (ageToRootStem < RealTime[0])
    error("tree stem not old enough for geo age");

  // Slice the phylo into periods defined by geo

  LineagePeriod = mem2d_i(p.nnodes, Times);
  
  double periodOld, periodYng, edgeOld,  edgeYng; 
  for (int i = 0; i < p.nnodes; i++)
    {
      for (int j = 0; j < Times; j++)
        {
          // expressed in ages
          periodOld = RealTime[j];
          periodYng = (j < Times-1) ? RealTime[j+1] : 0.0 ;
          edgeOld = p.age[i] + p.bl[i];
          edgeYng = p.age[i];

          // Rules:
          // if the edge passes through the period period beginning,
          //   and either spans the period of end in the period
          if ((edgeOld >= periodOld) && 
              ((edgeYng <= periodYng) || 
               ((edgeYng >= periodYng) && (edgeYng < periodOld))))
            LineagePeriod[i][j] = 1;
          /* printf(
             "phy%02d %4.1f -- %4.1f   slc%02d %4.1f -- %4.1f ---> [%d]\n",
             i, edgeOld, edgeYng, j, periodOld,  periodYng, 
             LineagePeriod[i][j]); */
        }
    }

  // create a parent lineage to daughter lineage array. Needed because some
  // edges have dissappeared if both beginning and ending within a period.
  
  LineageDaughters  = mem2d1_i(Lineages);
  LineageDaughtersN = mem1d_i(Lineages);

  for (int i = 0; i < Lineages; i++)
    for (int t = Times - 1; t > 0; t--) // note stepping until one before stem
      // if there is a LP at time t
      if (LineagePeriod[i][t])
        // is there a LP at time t-1?
        if (!LineagePeriod[i][t-1]) {
          // if not, check who is the parent:
          int node = p.parent[i];
          while(!LineagePeriod[node][t-1]) node = p.parent[node];
          /* printf("lin%2d time%2d is daughter of lin%2d time%2d\n",
             i, t, node, t-1);  */
          LineageDaughtersN[node] += 1;
          LineageDaughters[node] = (int *) realloc( LineageDaughters[node], 
                     LineageDaughtersN[node] * sizeof(int));
          LineageDaughters[node][LineageDaughtersN[node]-1]=i;
        }

  // Convert Extant (by taxa) to LineageExtant (by lineage)
  LineageExtant = mem3d_i(Lineages, Times, Spaces);
  int i;
  for (int x = 0; x < Taxa; x++) {
    for (i = 0; i < p.nnodes; i++)
      if (p.taxon[i])
        if (!strcmp(Taxon[x], p.taxon[i])) break;
    for (int t = 0; t < Times; t++)
      for (int s = 0; s < Spaces; s++)
        if (Extant[x][t][s])
          LineageExtant[i][t][s] = 1;
  }
}
