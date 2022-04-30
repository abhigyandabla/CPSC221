/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages
#include "ptree-private.h" // helper functions
#include <math.h> 
using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// Personal private helper functions
/////////////////////////////////

HSLAPixel PTree::getAverage(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  unsigned int numPixel = 0;
  HSLAPixel* curr;
  double avgH;
  double avgS;
  double avgL;
  double avghX;
  double avghY;
  double sumS = 0;
  double sumL = 0;
  double sumhX = 0;
  double sumhY = 0;
  unsigned int x = ul.first;
  while(x < w + ul.first) {
    unsigned int y = ul.second;
    while(y < h+ul.second) {
      curr = im.getPixel(x, y);
      sumS += curr->s;
      sumL += curr->l;
      sumhY += Deg2Y(curr->h);
      sumhX += Deg2X(curr->h);
      numPixel++;
      y++;
    }
    x++;
  }
  avgS = sumS / numPixel;
  avgL = sumL / numPixel;

  avghX = sumhX / numPixel;
  avghY = sumhY / numPixel;
  avgH = XY2Deg(avghX, avghY);
  return HSLAPixel(avgH, avgS, avgL);
}


Node* PTree::CopyHelper(Node* subRoot) {

  Node* newNode = new Node(subRoot->upperleft, subRoot->width, subRoot->height, subRoot->avg, subRoot->A, subRoot->B);

  if(subRoot->A != nullptr){
    newNode->A = CopyHelper(subRoot->A);
  }

  if(subRoot->B != nullptr){
    newNode->B = CopyHelper(subRoot->B);
  }

  return newNode;
}

int PTree::SizeHelper(Node* subRoot) const {
  if (subRoot == NULL) {
    return 0;
  } else {
    return 1 + SizeHelper(subRoot->A) + SizeHelper(subRoot->B);
  }
}

int PTree::NumLeavesHelper(Node* subRoot) const {
  if (subRoot == NULL) {
    return 0;
  } else {
    if (subRoot->A == NULL && subRoot->B == NULL) {
      return 1;
    } else {
      return NumLeavesHelper(subRoot->A) + NumLeavesHelper(subRoot->B);
    }
  }
}


void PTree::ClearHelper(Node* subRoot) {
  if (subRoot != nullptr) { 
    ClearHelper(subRoot->A);
    ClearHelper(subRoot->B);
    delete subRoot;
  }
  subRoot = nullptr;
}


/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
ClearHelper(root);
//root = NULL;
}


/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  root = CopyHelper(other.root);
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/

Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  int wA = w;
  int hA = h;
  int wB = w;
  int hB= h;
  pair<unsigned int, unsigned int> ulA = ul;
  pair<unsigned int, unsigned int> ulB = ul;
  if (w == 1 && h == 1){
    Node* leaf = new Node(ul, w, h, *im.getPixel(ul.first, ul.second), nullptr, nullptr);
    return leaf;
  }
  if(w < h){
  hA = h/2;
    hB = h -hA;
    ulB = make_pair(ul.first, (hA+ul.second));
  }
  else{
     wA = w/2;
    wB = w - wA;
  ulB = make_pair((wA + ul.first), ul.second);
  }
  HSLAPixel avgColor = PTree::getAverage(im, ulA, w, h);
  Node * childA = BuildNode(im,ulA,wA,hA);
  Node * childB = BuildNode(im,ulB,wB,hB);
  root = new Node(ul, w,h, avgColor,childA, childB);
  return root;
}
//   Node* curr = new Node(ul, w, h, getAverage(im, ul, w, h));
//   cout<<ul.second<<endl;
//   if (w != 1 || h != 1) {
//     //cout<<"c"<<endl;
//     pair<unsigned int, unsigned int> ulB;
//     unsigned int wA;
//     unsigned int hA;
//     unsigned int wB;
//     unsigned int hB;
    
//     if (w > h) {
//       hA = h; hB = h;
//       wA = floor(w / 2);
//       wB = w - wA;
//       ulB = make_pair(ul.first + wA, ul.second);
//     } else {
//       wA = w; wB = w;
//       hA = floor(h / 2);
//       hB = h - hA;
//       ulB = make_pair(ul.first, ul.second + hA);
//     }

//     curr->A = BuildNode(im, ul, wA, hA);
//     curr->B = BuildNode(im, ulB, wB, hB);
//   }
//   //cout<<"b"<<endl;
//   return curr;
// }

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  // add your implementation below
  root = BuildNode(im, make_pair(0, 0), im.width(), im.height());
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  Copy(other);
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  //const PTree& original = *this;
  if(this != &other) {
     Clear();
     Copy(other);
   }
   return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  Clear();
}
Node * PTree::RenderPruneHelper(Node*subRoot, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) const{
  int wA = w;
  int hA = h;
  int wB = w;
  int hB= h;
  pair<unsigned int, unsigned int> ulA = ul;
  pair<unsigned int, unsigned int> ulB = ul;
  if (w == 1 && h == 1){
    Node* leaf = new Node(ul, w, h, subRoot->avg, nullptr, nullptr);
    return leaf;
  }
  if(w < h){
    hA = h/2;
    hB = h -hA;
    ulB = make_pair(ul.first, (hA+ul.second));
  }
  else{
     wA = w/2;
    wB = w - wA;
    ulB = make_pair((wA + ul.first), ul.second);
  }
  Node * childA = RenderPruneHelper(subRoot,ulA,wA, hA);
  Node * childB = RenderPruneHelper(subRoot,ulB, wB, hB);
  Node* r = new Node(ul, w,h, subRoot->avg,childA, childB);
  //cout<<"End"<<endl;
  return r;
}
void PTree::RenderHelper(PNG* p, Node* subRoot) const{
  //cout<<"End"<<endl;
  if(subRoot == nullptr){
    return;
  }
  if(subRoot->A == nullptr && subRoot->B == nullptr){
    if(subRoot->width != 1 || subRoot->height != 1){
    subRoot = RenderPruneHelper(subRoot, subRoot->upperleft, subRoot->width, subRoot->height);
  }
    HSLAPixel * pixel = p->getPixel(subRoot->upperleft.first, subRoot->upperleft.second);
    pixel->h = subRoot->avg.h;
    pixel->s =subRoot->avg.s;
    pixel->l = subRoot->avg.l;
    pixel->a = subRoot->avg.a;

  }
  RenderHelper(p, subRoot->A);
  RenderHelper(p, subRoot->B);
}
//   Node * sub = subRoot;
//   if(subRoot == nullptr){
//     return;
//   }
//   if(sub->A == nullptr && subRoot->B == nullptr){
//     if(sub->width != 1 || sub->height != 1){
//     sub = RenderPruneHelper(subRoot, subRoot->upperleft, subRoot->width, subRoot->height);
//   }
//     HSLAPixel * pixel = p->getPixel(sub->upperleft.first, sub->upperleft.second);
//     pixel->h = sub->avg.h;
//     pixel->s =sub->avg.s;
//     pixel->l = sub->avg.l;
//     pixel->a = sub->avg.a;

//   }
//   RenderHelper(p, sub->A);
//   RenderHelper(p, sub->B);
//   if(sub != subRoot){
//     delete sub;
//     sub = nullptr;
//   }
// }
/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  unsigned int w = root->width;
  unsigned int h = root->height;
  PNG * img = new PNG(w , h);
  RenderHelper(img, root);
  PNG i = *img;
  delete img;
  return i;
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/



void PTree::PruneHelper(double tolerance, Node* subRoot) {
  if (subRoot != NULL) {

    // vector<Node*> leafNodes = GetLeafNodes(subRoot);
    // int prunable = -1;
    
    // for(Node* leaf : leafNodes) {
    //   if (leaf->avg.dist(subRoot->avg) > tolerance) {
    //     prunable = 0;
    //     break;
    //   } else {
    //     prunable = 1;
    //   }
    // }
    HSLAPixel subRootAvg = subRoot->avg;
    if (GetLeafNodes(subRoot, tolerance, subRootAvg)) {
      // delete subRoot->A;
      // delete subRoot->B;
      // subRoot->A = nullptr;
      // subRoot->B = nullptr;
      ClearHelper(subRoot->A);
      subRoot->A = NULL;
      ClearHelper(subRoot->B);
      subRoot->B = NULL;
     // return;
    } else {
      PruneHelper(tolerance, subRoot->A);
      PruneHelper(tolerance, subRoot->B);
    //}
  }
}
}

bool PTree::GetLeafNodes(Node * subRoot, double tolerance, HSLAPixel & avgP){
  bool within = true;
    if (subRoot!= NULL) {
        if (subRoot->A == NULL && subRoot->B == NULL) {
            double dist = avgP.dist(subRoot->avg);
			      within = (dist <= tolerance) && within;
        }
        else {
            bool left = GetLeafNodes(subRoot->A, tolerance, avgP);
            bool right = GetLeafNodes(subRoot->B, tolerance, avgP);
			      within = within && left && right;
        }
    }
    return within;
}

// vector<Node*> PTree::GetLeafNodes(Node* subRoot) {
//     vector<Node*> leafNodes;

//     if (subRoot->A == NULL && subRoot->B == NULL) {
//         leafNodes.insert(leafNodes.begin(), subRoot);
//     } else {
//       vector<Node*> aLeafNodes = GetLeafNodes(subRoot->A);
//       vector<Node*> bLeafNodes = GetLeafNodes(subRoot->B);
//       leafNodes.insert(leafNodes.end(), aLeafNodes.begin(), aLeafNodes.end());
//       leafNodes.insert(leafNodes.end(), bLeafNodes.begin(), bLeafNodes.end());
//     }

//     return leafNodes;
// }

void PTree::Prune(double tolerance) {
  //add your implementation below
  PruneHelper(tolerance, root);

}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  return SizeHelper(root);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  return NumLeavesHelper(root);
}

void PTree::FlipHHelper(unsigned int w, Node* subRoot){
  if(subRoot == nullptr){
    return;
  }
  pair<unsigned int, unsigned int> ulW;
  ulW.first = w - (subRoot->upperleft.first + subRoot->width);
  ulW.second = subRoot->upperleft.second;
  subRoot->upperleft = ulW;
  FlipHHelper(w, subRoot->A);
  FlipHHelper(w, subRoot->B);
}
/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  // add your implementation below
  FlipHHelper(root->width, root);
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVHelper(unsigned int h, Node* subRoot){
  if(subRoot == nullptr){
    return;
  }
  pair<unsigned int, unsigned int> ulW;
  ulW.second = h - (subRoot->upperleft.second + subRoot->height);
  ulW.first = subRoot->upperleft.first;
  subRoot->upperleft = ulW;
  FlipVHelper(h, subRoot->A);
  FlipVHelper(h, subRoot->B);
}
void PTree::FlipVertical() {
  // add your implementation below
  FlipVHelper(root->height, root);
  
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////