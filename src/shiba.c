#include "shiba.h"

int main(int argc, char *argv[])
{

  readXML();

  // printIndata();

  phylo p = parseNewick(Phylo[0]);

  // Free memory

  free2d_d(Area, Times);

  return 1;
}

