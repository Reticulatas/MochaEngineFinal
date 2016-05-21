#include "stdafx.h"
#include "CollisionTable.h"

bool CollisionTable::GetDoesIDsCollide( int id1, int id2 ) const
{
	return CollisionID[id1][id2];
}

void CollisionTable::SetDoesIDsCollide( int id1, int id2, bool doCollide )
{
	CollisionID[id1][id2] = doCollide;
}

CollisionTable::CollisionTable()
{
	for (unsigned i = 0; i < IDnum; ++i)
		for (unsigned j = 0; j < IDnum; ++j)
			CollisionID[i][j] = false;
	for (unsigned i = 0; i < IDnum; ++i)
		CollisionID[i][i] = true;
}

CollisionTable::CollisionTable( CollisionTable& o)
{
	for (unsigned i = 0; i < IDnum; ++i)
		for (unsigned j = 0; j < IDnum; ++j)
			CollisionID[i][j] = o.CollisionID[i][j];
}
