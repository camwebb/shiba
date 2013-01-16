SHIBA (Simulated Historical Island Biogeography Analysis) {#mainpage}
=========================================================

\htmlinclude version.txt

> `SHIBA` _is a software application that simulates the movement of taxa
> (from a pre-specified phylogeny) across a set of spaces that can shift
> location and size through time.<br/>It can thus use external geomorphological
> information to indicate the **likelihood of various biogeographical
> hypotheses**._ The use of `SHIBA` was described in Webb & Ree (2012).

These web pages document both the use of the executable and the C code
in which `SHIBA` is written.  They are generated from source documents
by Doxygen. The software and documentation are currently _under active
development_, and I would appreciate your
[feedback](mailto:cwebb@oeb.harvard.edu). There may still be 
bugs in the code.

<img src="shiba_eg1.jpg" align="right" width="350"/>

## Pages

 * \ref intro
 * \ref install
 * \ref infile
 * \ref usage
 * \ref pseudo of the biogeographical algorithms
 * \ref license
 * \ref ack

**References**

 * Webb, C. O., and R. Ree, 2012. Historical biogeography inference in
   Malesia.  Chapter 8, pages 191-215 in D. J. Gower, K. G. Johnson,
   J. E. Richardson, B. R. Rosen, L. Rüber, and S. T. Williams,
   editors. _Biotic Evolution and Environmental Change in Southeast
   Asia_. Cambridge University
   Press. [[PDF](http://camwebb.info/files/webb2012_sage.pdf)] [[book](http://www.cambridge.org/aus/catalogue/catalogue.asp?isbn=9781139533935)] 
 
\page ack Acknlowledgments

The initial idea and context for SHIBA arose during discussions
with Michael Donoghue, Rick Ree and Brian Moore in 2004, which also
led to Ree et al. (2005) and to [LAGRANGE](http://code.google.com/p/lagrange/).

**References**

 * Ree, R. H., B. R. Moore, C. O. Webb, and M. J. Donoghue. 2005. A
   likelihood framework for inferring the evolution of geographic
   range on phylogenetic trees. Evolution 59:2299-2311.

\page intro Introduction

## Overview

 * `SHIBA` combines information about i) the changing spatial
   conformation of land areas through time with ii) a dated phylogeny
   and iii) a contemporary distribution of terminal taxa in the land
   areas, in order to produce likelihoods of the spatial distribution
   of ancestral taxa. It achieves this through simulating the movement
   of ancestral taxa based on a simple model of island biogeography.
 * `SHIBA` models the history of a landscape as discrete ‘cells’ of
   time-`periods` and `spaces`.
 * Contemporaneous to this time-space grid is an ultrametric phylogeny
   with terminals in the present and a root stem than reaches at least
   until the beginning of the first time `period`.
 * The phylogeny is decomposed, or reconciled, with the space-time
   grid, so that each edge of the phylogeny graph is represented by a
   ‘`lineage`’ that begins and ends contemporaneously with the beginning
   and end of a time `period`.
    * (If a clade enters a `period` with on edge and leaves with two,
      the two ‘new’ lineages will be reconciled to appear at the
      beginning of the subsequent `period`.  If a clade enters a
      `period` with on edge and branchs twice, thus leaving with three
      edges, the three ‘new’ lineages will appear at
      the beginning of the subsequent `period`; information about the
      branch within a `period` does not persist).
 * Each run of the simulation:
    * Begins the root `lineage` with a starting distribution,
    * At each transition to the next period, the lineage may i)
      disperse to other `spaces`, with a probability based on the
      distance between `spaces`, and ii) survive or go extinct in each
      `space` it is in, with a probability based on the area of the
      `space`.
    * A successful simulation run is one where:
        * All `lineages` make it to the present, without dying out in
          all `spaces`.
        * The spatial distribution of taxa in the simulation is
          identical to the observed spatial distribution.
        * If observed fossils indicated that a particular ancestral lineage was
          in a particular place, then that lineage in the simulation
          must have passed through that historical place.
 * Successful runs are summarized (summed) to indicate the likelihood
   of ancestral `lineages` being in different places.

> `SHIBA` can best be seen as an experimental ‘sandbox’ to explore
> likelihoods of hypotheses about lineage movement. Varying parameter
> values and historical area details can then be used to assess the
> sensitivity of ones biogeographic conclusions.

## Input data

To run `SHIBA` you will need:

 * A ‘space-time’ grid, obtained from hypothetical geographical paleo-maps.
    * Start and end times for each `period`,
    * Areas for each historical `space` during each `period`,
    * Distances between each pair of historical `spaces` during each
      `period`.
 * A dated phylogeny with terminal taxa.
 * Information on the extant distribution of terminal taxa.

See \ref infile for more details on encoding your input data.

## Speciation-distribution model

`SHIBA` uses a simple model to determine the spatial distribution of
daughter lineages that maximises allopatric speciation. When, at the
transition of one period to the next, a lineage splitting occurs (as
specified by the input phylogeny):

 * First, one daughter lineage is assigned to each of the areas that
   the ancestral lineage was predicted to be present in, up to the
   number of daughter lineages. 
 * Then, for additional areas, a daughter lineages is chosen at random.

Hence, with two daughter lineages and two areas, the outcome will be
simple allopatry.  With two daughter lineages and one area, the
outcome will be simple sympatry. With two daughter lineages and three
areas, the outcome will be one area with one daughter lineage and two
areas with the other daughter lineage, indicating that the ancestral
species grew in spatial distribution, perhaps with little evolutionary
change, while the population in one area underwent divergence and
allopatry. Et cetera...

See \ref pseudo for more details of the biogeographical algorithm.

## Island Biogeography features

Both the size of an ancestral area and its degree of isolation from
other areas will strongly influence the liklihood that an ancestral
taxon distribution included it or not.  To incorporate these data,
`SHIBA` conatains a basic Island Biogeography model, with functions
similar to those proposed in the classic MacArthur and Wilson model:
extinction of a lineage on an island is inversely related to its area,
while dispersal probability into and out of an island is inversely
related to its isolation.

Please see \ref func for more details on the functions.


## Basic tutorial

 1. Please see \ref install and \ref usage first.
 2. Please examine the ascii-graphic at the top of the sample file in
    \ref infile. The example contains three areas. The ‘middle area,’ AreaB,
    begins by being attached to AreaA, moves away from A and ends by
    being attached to AreaC.  The example also contains two
    phylogenies, the first having branching events early (when A and B
    are close), the second  having branching events later (when B and C
    are close)
 3. Run `SHIBA` with the first phylogeny: `shiba -l -p 0` (the `-l`
    option give the input data printout). Look at the distribution of
    Lineage 0 and Lineage 2 (marked with a ‘`*`’ in the
    ascii-graphic).
 4. Now run `SHIBA` with the second phylogeny: `shiba -p 1`. Look
    again at the distribution of the Lineage 0 and 2.
 5. In both phylogenies, the root of the tree was in AreaA and AreaB,
    almost never in AreaC, indicating that the whole started on the
    ‘left’ (A, B in the image) and was ‘carried across’ to the ‘right’
    (B, C). It was too unlikely that it started in C but that Sp1 went
    extinct in C _and_ dispersed to A.
 6. In the first phylogeny, the majority of the runs have Lineage 2
    in AreaA and/or AreaB, while in the second, this lineage only
    occurs in AreaB.  The implication is that _with much time to go_,
    this lineage could have been widespread and then died out in A,
    but _with little time to go_ Sp1 and Sp2+Sp3 must have undergone a
    vicariant split prior at 20 Mya.
 7. By changing the survival and dispersal curves (with `-d` or `-s`
    on the command line, or in the `shibaInput.xml` file), our
    confidence in these interpretations can be further strenghtened.

## Performance

`SHIBA` use a ‘brute-force’ approach to finding successful solutions,
running each simulation until the present and then comparing observed
and simulated distributions.  This is very inefficient, in terms of
the number of solutions found relative to the number of runs initiated.
The code has been parallelized using `pthreads` to make use of all the
cores of a multi-core CPU. Nevertheless, with large trees and/or large
space-time grids, the program may take several hours to run.  By
directing the output to another file, and using `nohup`, the program
can be left to run on powerful servers:

      $ nohup shiba > out 

We are working to optimize the code further and to incorporate
alternative search heuristics.

## Further reading

Please see
[Webb & Ree (2012)](http://camwebb.info/files/webb2012_sage.pdf) for
more details.

\page func Island Biogeography functions

<img align="right" src="s+d.jpg" width="400"/>

## Survival function 

We use the logarithm function as the basic survival function for an
island \f$i\f$ with area \f$A_i\f$: \f$log(A_i)\f$

We standardize this by using proportions of total land area: \f$log(\frac{A_i}{A_{total}})\f$

The curve can be forced through `(0,0)` and `(1,1)`: \f$log(\frac{A_i
\times 9}{A_{total}} + 1)\f$

Increasingly steeper a `(0,0)` are \f$log(\frac{A_i \times 99}{A_{total}}
+ 1) / 2\f$ and \f$log(\frac{A_i \times 999}{A_{total}} + 1) / 3\f$

Generalizing, the basic survival curve is: \f$log(\frac{A_i \times (10^b - 1)}{A_{total}} + 1) / b\f$

Finally, a further parameter, \f$p_{surv}\f$ sets the survival for
\f$A_{total}\f$:

> **Survival probablility** \f$= p_{surv} \times log(\frac{A_i \times (10^b - 1)}{A_{total}} + 1) / b\f$

In the **input data**, \f$p_{surv}\f$ is set with `probSurvA` and shape
parameter \f$b\f$ is set with `probSurvB`. Values of ‘1.0’ for
`probSurvA` and ‘2.0’ for `probSurvB` are appropriate starting
values. You can view the shape of the curve with the ‘`-l`’ flag.

## Dispersal function 

For dispersal, we use a negative power function dependent on the
distance between two places (\f$d_{i,j}\f$), standardized by maximum
distance between any two places (\f$d_{max}\f$): \f$10^{(-1 \times
\frac{d_{i,j}}{d_{max}})}\f$

The concavity of the curve (steepness at `(0,1)`) can best be adjusted
with a parameter \f$b\f$ (as opposed to varying the ‘base,’ i.e. 10):
\f$10^{(-1 \times b \times \frac{d_{i,j}}{d_{max}})}\f$

Finally, a further parameter, \f$p_{disp}\f$ sets the dispersal
probablility for distance zero, i.e., when landmasses are touching.

> **Dispersal probablility** \f$= p_{disp} \times 10^{(-1 \times b \times \frac{d_{i,j}}{d_{max}})}\f$

In the **input data**, \f$p_{disp}\f$ is set with `probDispA` and shape
parameter \f$b\f$ is set with `probDispB`. Values of ‘1.0’ for
`probDispA` and ‘2.0’ for `probDispB` are appropriate starting
values. You can view the shape of the curve with the ‘`-l`’ flag.


\page install Installation

`SHIBA` code is hosted at [github](https://github.com/camwebb/shiba)
and is not currently distributed as pre-compile binaries.  Please
follow these instructions to install. 

## Installation on Linux

 * On the github page, click on the `ZIP` button to download a
   snapshot of the source. You can of course also use `git` to clone
   the repo if you have it installed.
 * There is one dependency: the [Mini-XML](http://www.minixml.org/)
   development libraries. Your Linux distro may have this included in
   the core package management system; search for `mxml` and install
   it.  It is also easy to install directly; see instructions on the
   [Mini-XML](http://www.minixml.org/documentation.php/install.html)
   site.
 * There is currently no `configure` script, but if you have the (most
   common) `gcc` compiler installed, you should just be able to type:
   `make` in the `src` directory.  The resulting `shiba` binary can
   then be moved anywhere (on your system) needed.

\page infile Input File Format

Because of the complexity of the data required as input, I chose XML
as the most appropriate input file format. For small cases, it is
probably easiest to write the XML ‘by hand’ (i.e., in an XML-aware
text editor, e.g. emacs with nXML). For larger datasets, you can
generate the repetitive lines with a script, or even with concatenate
functions in a spreadsheet. Note that the XML
attributes identifying the spaces, times, etc can be any text strings,
so can actually be the same as the names of the taxa, spaces, etc.

While it may take a bit of work to get the data into this format, the
great benefit of XML is that it can be validated against a schema.  I
suggest [trang](http://www.thaiopensource.com/relaxng/trang.html) and 
[jing](http://www.thaiopensource.com/relaxng/jing.html):

      $ java -jar trang.jar shibaInput.rnc shibaInput.rng
      $ java -jar jing.jar shibaInput.rng shibaInput.xml

## Example XML file (`shibaInput.xml`)

\include shibaInput.xml

## Relax NG Compact schema (`shibaInput.rnc`)

\include shibaInput.rnc


\page usage Usage

After \ref install, the program can be run simply by typing `shiba` in
a terminal window, while in the same directory it was compiled in.  If
you have not set your `PATH` environmental variable to include ‘`.`’,
you will need to type `./shiba`. The otions area:
 
      -d NUM   Use this value as the prob. of dispersal (probDispA)
      -f FILE  Use this file as input. Default file: shibaInput.xml
      -h       Print this help list
      -l       Print input data summary
      -p INT   Use this phylogeny (0...n). Default: 0
      -s NUM   Use this value as the prob. of survival (probSurvA)
      -v       Print highly verbose event descriptions

## Output format

      ## Successes : 102 (in 34000 runs)
      
      ## Number of runs in which the base of each lineage was in each space:
         (*= extant distrib)
      
          Space       |     0    1    2 
          ------------+-----------------
          Lineage   0 |    51   46    5   --> l1 l2 
          Lineage   1 |    66*  31    5   --> Sp1
          Lineage   2 |    36   60    6   --> l3 l4 
          Lineage   3 |    22   71*   9*  --> Sp2
          Lineage   4 |    28   66*   8*  --> Sp3

### In words

“The base of `Lineage 0` (the basal lineage, in the first
time period) occurred in 51 of the 102 successful runs in `Area 0`,
and in only 5 of the runs in `Area 2`, hence indicating that it was
more likely that the whole clade originated in the first and second
areas than in the third.  `Lineage 0` splits to form `lineages 1 and
2`.  `Lineage 1` is that which ends in extant taxon `Sp1` and started
predominantly in the first area. The `*` indicates the extant
distribution of `Sp1`, which shows that runs in which the base of
`lineage 1` began in the second and third areas must have included the
extinction of that taxon in those areas before the present.”

\page license License

**Copyright (c) 2013, Campbell Webb**
   
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

