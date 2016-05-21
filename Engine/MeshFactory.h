/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Kenneth Tan
  -* See Footer for Revisions                                      */


#pragma once
#include "Mesh.h"
#include "LineMesh.h"

#define MAXPARTICLEBUFFER 5000

class MeshFactory
{

public:

  ~MeshFactory();
	Mesh* CreateNewQuadMesh();
	Mesh* CreateBoxMesh();
	Mesh* CreateDebugMesh();
	Mesh* CreateParticleMesh(unsigned maxParticles);
	Mesh* CreateTextMesh(unsigned maxCharacters);
  Mesh* CreatePPSQuad();
	static MeshFactory* getInstance();
};

/////////////////////////////////////
//Original Author: Kenneth Tan 
//Modifications:
//	Date	-	Change