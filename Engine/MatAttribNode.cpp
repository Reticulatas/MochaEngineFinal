/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "MatAttribNode.h"

void MatAttribNode::BindAttribute( Shader* shader )
{

}

MatAttribNode::~MatAttribNode()
{

}

bool MatAttribNode::operator==( const MatAttribNode& rhs )
{
	return (this->mymeta().gettoplevelname() == rhs.mymeta().gettoplevelname());
}

meta_define(MatAttribNode);

/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change