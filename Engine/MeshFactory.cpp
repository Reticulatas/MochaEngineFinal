/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "MeshFactory.h"

Mesh* MeshFactory::CreateNewQuadMesh()
{
	Vertex v1(Vector3(-0.5f, +0.5f, 0), Vector2(0, 0)),
		     v2(Vector3(+0.5f, +0.5f, 0), Vector2(1, 0)),
		     v3(Vector3(-0.5f, -0.5f, 0), Vector2(0, 1)),
		     v4(Vector3(+0.5f, -0.5f, 0), Vector2(1, 1));

	Mesh* quad = new Mesh("Quad.mesh");
	quad->SetType("Meshes");
    
	quad->BeginMesh();

		/*initialize Vertices */
		quad->AddVertex(v1);
		quad->AddVertex(v2);
		quad->AddVertex(v3);
		quad->AddVertex(v4);

		/* Initialize Indices */

		// FRONT
		quad->AddIndex(2);
		quad->AddIndex(0);
		quad->AddIndex(1);
		quad->AddIndex(2);
		quad->AddIndex(1);
		quad->AddIndex(3);

    quad->ComputeNormal();

	quad->EndMesh(Renderer::getInstance()->GetD3D11Device());

	return quad;
}

Mesh* MeshFactory::CreateBoxMesh()
{
	Vertex v0(Vector3(-0.5f, +0.5f, -0.5f), Vector2(0, 0)),
		     v1(Vector3(+0.5f, +0.5f, -0.5f), Vector2(1, 0)),
		     v2(Vector3(-0.5f, -0.5f, -0.5f), Vector2(0, 1)),
		     v3(Vector3(+0.5f, -0.5f, -0.5f), Vector2(1, 1)),
				  		
		     v4(Vector3(-0.5f, +0.5f, +0.5f), Vector2(0, 0)),
		     v5(Vector3(+0.5f, +0.5f, +0.5f), Vector2(1, 0)),
		     v6(Vector3(-0.5f, -0.5f, +0.5f), Vector2(0, 1)),
		     v7(Vector3(+0.5f, -0.5f, +0.5f), Vector2(1, 1));

	Mesh* box = new Mesh("Box.mesh");
	box->SetType("Meshes");

	box->BeginMesh();

		/*initialize Vertices */
		box->AddVertex(v0);
		box->AddVertex(v1);
		box->AddVertex(v2);
		box->AddVertex(v3);
		box->AddVertex(v4);
		box->AddVertex(v5);
		box->AddVertex(v6);
		box->AddVertex(v7);

		/* Initialize Indices */

		// FRONT
		box->AddIndex(2);
		box->AddIndex(0);
		box->AddIndex(1);
		box->AddIndex(2);
		box->AddIndex(1);
		box->AddIndex(3);
		
		// TOP
		box->AddIndex(1);
		box->AddIndex(0);
		box->AddIndex(4);
		box->AddIndex(1);
		box->AddIndex(4);
		box->AddIndex(5);

		// BACK
		box->AddIndex(7);
		box->AddIndex(5);
		box->AddIndex(4);
		box->AddIndex(7);
		box->AddIndex(4);
		box->AddIndex(6);

		// BOTTOM
		box->AddIndex(2);
		box->AddIndex(3);
		box->AddIndex(7);
		box->AddIndex(2);
		box->AddIndex(7);
		box->AddIndex(6);

		// RIGHT
		box->AddIndex(1);
		box->AddIndex(5);
		box->AddIndex(7);
		box->AddIndex(1);
		box->AddIndex(7);
		box->AddIndex(3);

		// LEFT
		box->AddIndex(2);
		box->AddIndex(4);
		box->AddIndex(0);
		box->AddIndex(2);
		box->AddIndex(6);
		box->AddIndex(4);

		box->ComputeNormal();

	box->EndMesh(Renderer::getInstance()->GetD3D11Device());

	return box;
}

MeshFactory* MeshFactory::getInstance()
{
	static MeshFactory* instance = 0;

	if(!instance)
		instance = new MeshFactory();

	return instance;
}

Mesh* MeshFactory::CreateDebugMesh()
{
  LineMesh* line = new LineMesh("DebugMesh", true);
  LineVertex v;
  line->SetType("Meshes");

  line->BeginMesh();

  line->GetLineVertices().resize(MAXLINES);

  line->EndMesh(Renderer::getInstance()->GetD3D11Device());

  return line;
}

Mesh* MeshFactory::CreateParticleMesh(unsigned maxParticles)
{
  unsigned bufferSize = maxParticles * 6;

  Mesh* mParticleMesh = new Mesh("ParticleMesh", true);
  mParticleMesh->SetType("Meshes");
  Vertex v0(Vector3(-0.5f, +0.5f, -0.5f), Vector2(0, 0));

  mParticleMesh->BeginMesh();

  for(unsigned  i = 0; i < bufferSize; ++i)
    mParticleMesh->AddVertex(v0);

  for(unsigned i = 0; i < bufferSize * 3; ++i)
    mParticleMesh->AddIndex(0);

  mParticleMesh->EndMesh(Renderer::getInstance()->GetD3D11Device());

  return mParticleMesh;
}

Mesh* MeshFactory::CreateTextMesh( unsigned maxCharacters )
{
	unsigned bufferSize = maxCharacters * 6;

	Mesh* mTextMesh = new Mesh("TextMesh", true);
	mTextMesh->SetType("Meshes");

	Vertex v0(Vector3(-0.5f, +0.5f, -0.5f), Vector2(0, 0));

	mTextMesh->BeginMesh();

	for(unsigned  i = 0; i < bufferSize; ++i)
		mTextMesh->AddVertex(v0);

	for(unsigned i = 0; i < bufferSize * 3; ++i)
		mTextMesh->AddIndex(0);

	mTextMesh->EndMesh(Renderer::getInstance()->GetD3D11Device());

	return mTextMesh;
}

Mesh* MeshFactory::CreatePPSQuad()
{
  Mesh* ppsQuad = new Mesh("PPSQuad.mesh", true);
  ppsQuad->SetType("Meshes");

  Vertex v0(Vector3(-0.5f, +0.5f, -0.5f), Vector2(0, 0));

  ppsQuad->BeginMesh();

  for(unsigned  i = 0; i < 4; ++i)
    ppsQuad->AddVertex(v0);

  for(unsigned i = 0; i < 6; ++i)
    ppsQuad->AddIndex(0);

  ppsQuad->EndMesh(Renderer::getInstance()->GetD3D11Device());

  return ppsQuad;
}