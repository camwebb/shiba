#define LINEAGES 9       // Used in solving only

// My
//                       5Ma time period beginning 55Ma
// 12            0
// 11     1..........6
// 10     1          6
// 9  2--+-3        6 
// 8  2    3        6 
// 7  2    3        6 
// 6  2    3    7---+8
// 5  2  4-+-5  7    8
// 4  2  4   5  7    8
// 3  2  4   5  7    8
// 2  2  4   5  7    8
// 1  2  4   5  7    8
//
//     ^tuba:ngu
//        ^pudor:wsu 
//            ^polyant:bor
//               ^javaan:luz+phi+borne+wsul
//                     ^kawak:bor

int live[LINEAGES][TIME] = { \
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},			\
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},			\
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
  {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},	\
  {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},	\
  {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},	\
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1} \
};

int daughters[LINEAGES][2] = { \
  {1, 6}, \
  {2, 3}, \
  {0, 0}, \
  {4, 5}, \
  {0, 0}, \
  {0, 0}, \
  {7, 8}, \
  {0, 0}, \
  {0, 0}  \
};

int extant[LINEAGES][SPACE] = {    \
// bo ws ng ph lz
  {0, 0, 0,  0},	   			   \
  {0, 0, 0,  0},				   \
  {0, 0, 1, 0}, /* tuba */	   \
  {0, 0, 0, 0},            	   \
  {0, 1, 0, 0}, /* pudor */	   \
  {1, 0, 0, 0}, /* polyant */   \
  {0, 0, 0, 0},				   \
  {0, 0, 0, 1}, /* quad */	   \
  {1, 0, 0, 0}  /* kawak */	   \
};

// The possible start areas:
int constrains[SPACE] = {1, 1, 1, 1};
// A constrint on areas that can be dispersed into. Not sure why I made this
int constraind[SPACE] = {1, 1, 1, 1};

