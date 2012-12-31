#include "shiba.h"

int main(int argc, char *argv[])
{

  readXML();
  printf("Time: %d\n", Times);
  printf("Spaces: %d\n", Spaces);
  printf("Phylos: %d\n", Phylos);
  printf("Taxa: %d\n", Taxa);
  printf("Time: %s - %f\n", TimeLabel[2], RealTime[2]);
  printf("Space: %s - %s\n", SpaceLabel[0], SpaceName[0]);
  printf("Phylo: %s - %s\n", PhyloLabel[0], Phylo[0]);
  printf("Taxon: %s - %s\n", TaxonLabel[2], Taxon[2]);

  return 1;
}

