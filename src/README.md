SHIBA (Simulated Historical Island Biogeography Analysis) {#mainpage}
=========================================================

> `SHIBA` _is a software application that simulates the movement of taxa
> (from a pre-specified phylogeny) across a set of spaces that can shift
> location and size through time.<br/>It can thus use external geomorphological
> information to indicate the **likelihood of various biogeographical
> hypotheses**._

The use of `SHIBA` was described in:

 * Webb, C. O., and R. Ree, 2012. Historical biogeography inference in
   Malesia.  Chapter 8, pages 191-215 in D. J. Gower, K. G. Johnson,
   J. E. Richardson, B. R. Rosen, L. Rüber, and S. T. Williams,
   editors. _Biotic Evolution and Environmental Change in Southeast
   Asia_. [Cambridge University Press](http://www.cambridge.org/aus/catalogue/catalogue.asp?isbn=9781139533935).

These web pages document both the use of the executable and the C code
in which `SHIBA` is written.  They are generated from source documents
by Doxygen.

 * \ref install
 * \ref infile
 * \ref usage


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

      Usage: shiba [ -h -f FILE -p INT ]
      
      Options:
         -f     Use this file as input. Default file: shibaInput.xml
         -p     Use this phylogeny (0...n). Default: 0
         -h     Print this help list

-----


  


