/*! \file 
 * **The main() function**
 */

#include "shiba.h"

//! The main program controller

int main(int argc, char **argv)
{
  phylo p;
  // Set defaults for global variables
  PhyloToUse = 0;
  DataFile = "shibaInput.xml";
  PrintData = 0;

  // Read arguments
  readArgs(argc, argv);
  // Read datafile
  readXML();

  // Parse phylogeny
  if ((PhyloToUse >= 0) && (PhyloToUse < Phylos))
    p = parseNewick(Phylo[PhyloToUse]);
  else error("Specified phylo number does not match a phylo in input file");
  phyloToLineage(p);

  if (PrintData) printIndata();


  // Free memory

  free(p.parent);
  free(p.ndaughter);
  free(p.depth);
  free(p.age);
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
  free2d_i(LineagePeriod, p.nnodes);
  free(LineageNDaughters);
  free2d1_i(LineageDaughters, p.nnodes);

  return 1;
}

/*! Parses arguments to the executable. Pass argc and argv directly from main()
 *  . copied from the GNU libc `getopt()`
 * [example](http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html).
 * \param argc The number of arguments
 * \param argv The array of strings of the various arguments
 */

void readArgs(int argc, char **argv)
{
  int c;
  opterr = 0;
     
  while ((c = getopt (argc, argv, "f:hp:l")) != -1)
    switch (c)
      {
      case 'f':
        DataFile = optarg;
        break;
      case 'l':
        PrintData = 1;
        break;
      case 'p':
        PhyloToUse = atoi(optarg);
        break;
      case 'h':
        help();
      case '?':
        if (optopt == 'f')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
      default:
        abort();
      }
}
