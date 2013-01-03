#include "shiba.h"

int main(int argc, char *argv[])
{

  readXML();

  // printIndata();

  phylo p = parseNewick(Phylo[0]);

  // Free memory

  free(p.parent);
  free(p.ndaughter);
  free(p.depth);
  free(p.bl);
  free2d1_c(p.taxon, p.nnodes);


  free2d_d(Area, Times);
  free3d_d(Dist, Times, Spaces);
  free(RealTime);
  free2d1_c(SpaceName, Spaces);
  free2d1_c(Phylo, Phylos);
  free2d1_c(Taxon, Taxa);
  free3d_i(Extant, Taxa, Times);
  free(Cfg.startSpace);


  return 1;
}

