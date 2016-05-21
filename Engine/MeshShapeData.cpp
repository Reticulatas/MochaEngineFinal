#include "stdafx.h"
#include "MeshShapeData.h"
ShapeData::ShapeData(std::string fileName)
{
	std::ifstream shapeFile;
	shapeFile.open(fileName, std::ifstream::in);
	if (shapeFile)
	{
		unsigned numShapes = 0;
		shapeFile >> numShapes;
		for (unsigned i = 0; i < numShapes; ++i)
		{
			//aquire verticies
			unsigned numVerticies = 0;
			std::vector<Vector> shape;
			shapeFile >> numVerticies;
			for (unsigned j = 0; j < numVerticies; ++j)
			{
				Vector vert;
				shapeFile >> vert.x;
				shapeFile >> vert.y;
				shapeFile >> vert.z;
				vert.w = 1.0f;
				shape.push_back(vert);
			}
			if (shape.size() > 0)
			{
				m_shapeVerts.push_back(shape);
			}
		}
		for (unsigned i = 0; i < numShapes; ++i)
		{
			//aquire indicies
			unsigned numIndicies = 0;
			std::vector<unsigned> indicies;
			shapeFile >> numIndicies;
			for (unsigned j = 0; j < numIndicies; ++j)
			{
				unsigned index;
				shapeFile >> index;
				indicies.push_back(index);
			}
			if (indicies.size() > 0)
			{
				m_shapeIndicies.push_back(indicies);
			}
		}
		//close the file
		shapeFile.close();
	}
	else
	{
		std::printf("Error: Failed to load shape data from %s\n", fileName.c_str());
	}
}

unsigned ShapeData::GetNumShapes()
{
	return m_shapeVerts.size();
}

std::vector<Vector> ShapeData::GetVerticies(unsigned shapeNum)
{
	return m_shapeVerts[shapeNum];
}

std::vector<unsigned> ShapeData::GetIndicies(unsigned shapeNum)
{
	return m_shapeIndicies[shapeNum];
}