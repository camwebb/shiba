/*! \file
 * Input, output and error functions
 */

#define _GNU_SOURCE
#include <mxml.h>
#include <string.h>
#include "shiba.h"

/*!
 * Open file and parse XML; 
 * derived from mini-xml website examples
 */

void readXML()
{

  FILE *fp;
  mxml_node_t *tree;
  mxml_node_t *node;
  int t = 0; int s = 0; int p = 0; int x = 0;

  //double datad;

  fp = fopen("infile.xml", "r");
  tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
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

  // ------------- Dimension arrays in the heap -----------------------------

  RealTime = dvector(0, Times-1); 
  if ( (TimeLabel = (char**) malloc( Times * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to TimeLabel");
  if ( (SpaceName = (char**) malloc( Spaces * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to SpaceName");
  if ( (SpaceLabel = (char**) malloc( Spaces * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to SpaceLabel");
  if ( (Phylo = (char**) malloc( Phylos * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to Phylo");
  if ( (PhyloLabel = (char**) malloc( Phylos * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to PhyloLabel");
  if ( (Taxon = (char**) malloc( Taxa * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to Taxon");
  if ( (TaxonLabel = (char**) malloc( Taxa * sizeof(char*))) == NULL ) \
      error("Cannot allocate memory to TaxonLabel");

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
              if (!mxmlGetText(mxmlGetFirstChild(node),0)) \
                error("A periodStart element is missing");
              else RealTime[t] = atof(mxmlGetText(mxmlGetFirstChild(node),0));
              if (!mxmlElementGetAttr(node, "id"))            \
                error("An id attr is missing in periodStart");
              else asprintf(&TimeLabel[t], "%s", \
                mxmlElementGetAttr(node, "id"));
              t++;
            }

          // spaceName elements
          else if (!strcmp(mxmlGetElement(node), "spaceName"))
            {
              if (s >= Spaces) error("Too many spaceName elements");
              if (!mxmlGetText(mxmlGetFirstChild(node),0)) \
                error("A spaceName datum is missing");
              else asprintf(&SpaceName[s], "%s", \
                mxmlGetText(mxmlGetFirstChild(node),0));
              if (!mxmlElementGetAttr(node, "id"))            \
                error("An id attr is missing in spaceName");
              else asprintf(&SpaceLabel[s], "%s", \
                mxmlElementGetAttr(node, "id"));
              s++;
            }

          // phylo elements
          else if (!strcmp(mxmlGetElement(node), "newick"))
            {
              if (p >= Phylos) error("Too many newick elements");
              if (!mxmlGetText(mxmlGetFirstChild(node),0))  \
                error("A newick datum is missing");
              else asprintf(&Phylo[p], "%s",            \
                mxmlGetText(mxmlGetFirstChild(node),0));
              if (!mxmlElementGetAttr(node, "id"))          \
                error("An id attr is missing in newick");
              else asprintf(&PhyloLabel[p], "%s", \
                mxmlElementGetAttr(node, "id"));
              p++;
            }

          // taxa elements
          else if (!strcmp(mxmlGetElement(node), "taxon"))
            {
              if (x >= Taxa) error("Too many taxon elements");
              if (!mxmlGetText(mxmlGetFirstChild(node),0))  \
                error("A taxon datum is missing");
              else asprintf(&Taxon[x], "%s",                \
                mxmlGetText(mxmlGetFirstChild(node),0));
              if (!mxmlElementGetAttr(node, "id"))          \
                error("An id attr is missing in taxon");
              else asprintf(&TaxonLabel[x], "%s", \
                mxmlElementGetAttr(node, "id"));
              x++;
            }

        }

      node = mxmlWalkNext(node, tree, MXML_DESCEND);
        
    }

  // Check for correct dimensioning
  if (t != Times) error("Wrong number of periodStart elements");
  if (s != Spaces) error("Wrong number of spaceName elements");
  if (p != Phylos) error("Wrong number of newick elements");
  if (x != Taxa) error("Wrong number of taxon elements");
  
  /* cur = xmlDocGetRootElement(doc); */
  /* if (!cur) error("Could not get root element");  */

  /* if (xmlStrcmp(cur->name, (const xmlChar *) "shibaInput")) \ */
  /*   error("document of the wrong type, root node != shibaInput"); */

  /* // begin the crawl */
  /* cur = cur->xmlChildrenNode; */
  /* while (cur != NULL) */
  /*   { */
  /*     if ((!xmlStrcmp(cur->name, (const xmlChar *) "time"))) */
  /*       { */
  /*         cur = cur->xmlChildrenNode; */
  /*         int i = 0; */
  /*         while (cur != NULL) { */
  /*           if (!xmlStrcmp(cur->name, (const xmlChar *) "periodStart")) */
  /*             { */
  /*               RealTimes[i] = atof((char *) xmlNodeListGetString(doc, cur->xmlChildrenNode, 1)); */
  /*             } */
  /*   	    printf("keyword: %f\n", RealTimes[i]); */
  /*           i++; */
  /*           cur = cur->next; */
  /*         } */
  /*   	} */
      
  /*     cur = cur->next; */
  /*   } */
	


  /* /\* printf("Result (%d nodes):\n", size); *\/ */
  /* /\* for(int i = 0; i < size; ++i) { *\/ */
	
  /* /\* if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) { *\/ */
  /* /\*     cur = nodes->nodeTab[i];   	     *\/ */
  /* /\*     if(cur->ns) {  *\/ */
  /* /\*       printf("= element node \"%s:%s\"\n",  *\/ */
  /* /\*               cur->ns->href, cur->name); *\/ */
  /* /\*     } else { *\/ */
  /* /\*       printf("= element node \"%s\"\n",  *\/ */
  /* /\*               cur->name); *\/ */
  /* /\*     } *\/ */
  /* /\*   } else { *\/ */
  /* /\*     cur = nodes->nodeTab[i];     *\/ */
  /* /\*     printf("= node \"%s\": type %d\n", cur->name, cur->type); *\/ */
  /* /\*   } *\/ */
  /* /\* } *\/ */


  /* /\* Print results *\/ */
  /* // print_xpath_nodes(xpathObj->nodesetval, stdout); */
  
  /* /\* Cleanup *\/ */
  /* xmlXPathFreeObject(xpathObj); */
  /* xmlXPathFreeContext(xpathCtx);  */
  /* xmlFreeDoc(doc);  */
    
  /* /\* // Set some scalars *\/ */
  /* /\* cfg.time = cJSON_GetObjectItem(root,"time")->valueint; *\/ */
  /* /\* cfg.space = cJSON_GetObjectItem(root,"space")->valueint; *\/ */

  /* /\* // Set vectors *\/ */
  /* /\* int jspace = cJSON_GetArraySize(cJSON_GetObjectItem(root,"spacenames")); *\/ */
  /* /\* if (jspace != cfg.space)  *\/ */
  /* /\*   { *\/ */
  /* /\*     error("Number of space names not the same as declared areas") ; *\/ */
  /* /\*   } *\/ */
  /* /\* // Allocate memory to pointers to spacenames: *\/ */
  /* /\* if (( cfg.spacenames = ( char** ) malloc( jspace*sizeof( char* ))) == NULL ) *\/ */
  /* /\*   {  *\/ */
  /* /\*     error("Cannot allocate memory to spacenames"); *\/ */
  /* /\*   } *\/ */

  /* /\* for (int i = 0; i < jspace; i++) *\/ */
  /* /\*   { *\/ */
  /* /\*     asprintf(&cfg.spacenames[i], "%s", \ *\/ */
  /* /\*       cJSON_GetArrayItem(              \ *\/ */
  /* /\*         cJSON_GetObjectItem(root,"spacenames"), i)->valuestring ); *\/ */
  /* /\*   } *\/ */

  /* /\* free(data); *\/ */
  /* /\* cJSON_Delete(root); *\/ */


mxmlDelete(tree);

}

void error(char *error_msg)
{
  printf("ERROR:\n  %s\n", error_msg);
  exit(0);
  // NB: In the tcsh, `echo $status` returns the exit status of 
  // the preceding command.
}
