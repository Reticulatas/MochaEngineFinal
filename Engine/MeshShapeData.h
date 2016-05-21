#pragma once
#include <string>
#include <vector>
#include "MathIncludes.h"
class ShapeData
{
public:
	ShapeData(std::string fileName);
	unsigned GetNumShapes();
	std::vector<Vector> GetVerticies(unsigned shapeNum);
	std::vector<unsigned> GetIndicies(unsigned shapeNum);
private:
	std::vector<std::vector<Vector> > m_shapeVerts;
	std::vector<std::vector<unsigned > > m_shapeIndicies;
};