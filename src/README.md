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
[feedback](mailto:cwebb@oeb.harvard.edu). There may still be serious
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

(Coming soon. For the moment, please see [Webb & Ree 2012](http://camwebb.info/files/webb2012_sage.pdf))

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

    Usage: shiba [ -dfhlpsv ]
    Options:
      -d NUM   Use this value as the prob. of dispersal modifier
      -f FILE  Use this file as input. Default file: shibaInput.xml
      -h       Print this help list
      -l       Print input data summary
      -p INT   Use this phylogeny (0...n). Default: 0
      -s NUM   Use this value as the prob. of survival modifier
      -v       Print highly verbose event descriptions

(more to come...)

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

