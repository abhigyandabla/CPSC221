/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

#include "cs221util/PNG.h"
#include "cs221util/HSLAPixel.h"

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////

HSLAPixel getAverage(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h);
Node* CopyHelper(Node* subRoot);
int SizeHelper(Node* subRoot) const;
int NumLeavesHelper(Node* subRoot) const;
void ClearHelper(Node* subRoot);
void FlipHHelper(unsigned int w, Node* root);
void FlipVHelper(unsigned int h, Node* root);
void PruneHelper(double tolerance, Node* subRoot);
bool GetLeafNodes(Node * subRoot, double tolerance, HSLAPixel & avgP);
void RenderHelper(PNG* p, Node* subRoot) const;
Node* RenderPruneHelper(Node*subRoot, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) const;
#endif