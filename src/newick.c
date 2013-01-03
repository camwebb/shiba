/*! \file
 * Parses newick tree strings
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

  // Move through the newick format tree character by character 
  int i = 0;
  // start outside parentheses, one hypothetical node proximal to root node
  int node = -1;
  int parent = -2;
  int nodeCount = 0;
  while (i < strlen(in))
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
         
          char *tmp; int tmpn = 0;
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
          
              // check for name, I.e. not `:' nor `['
              while ((in[i] != 58) && (in[i] != 91) && 
                     (in[i] != 44) && (in[i] != 41) && (in[i] != 40))
                {
                  if (!tmpn) {
                    asprintf(&tmp, "%c", in[i]); tmpn++;
                  } else {
                    asprintf(&tmp, "%s%c", tmp, in[i]); tmpn++;
                  }
                  i++;
                }
              // was there any taxon name?
              asprintf(&p.taxon[node], "%s", tmp);
              // printf("  taxon name %s\n", tmp);
            }
          
          // are there bls?
          if (in[i] == 58)
            {
              i++; // skip over delimiter `:'
              tmpn = 0;
              // bead bl string, I.e. not `['
              while ((in[i] != 91) &&
                     (in[i] != 44) && (in[i] != 41) && (in[i] != 40))
                {
                  if (!tmpn) {
                    asprintf(&tmp, "%c", in[i]); tmpn++;
                  } else {
                    asprintf(&tmp, "%s%c", tmp, in[i]); tmpn++;
                  }
                  i++;
                }
              p.bl[node] = atof(tmp);
              // printf("  taxon bl %f\n", p.bl[node]);
            }
          
          // are there notes?
          if (in[i] == 91)
            // Discard note
            while ((in[i] != 44) && (in[i] != 41) && (in[i] != 40))
              i++;
        }
    }

  /* printf("%s\n", in);
  for (int n = 0; n < p.nnodes; n++)
      printf("%3d %3d %3d %3d %5.2f %s\n", n, p.parent[n], p.depth[n], 
      p.ndaughter[n], p.bl[n], p.taxon[n]); */

  return p; 
}
