#pragma once

class CollisionTable
{
public:
	const static unsigned IDnum = 10;
private:
	bool CollisionID[IDnum][IDnum];
public:
	CollisionTable();
	CollisionTable(CollisionTable&);

	bool GetDoesIDsCollide(int id1, int id2) const;
	void SetDoesIDsCollide(int id1, int id2, bool doCollide);

	unsigned GetWidth() const { return IDnum; }
	unsigned GetHeight() const { return IDnum; }
};

