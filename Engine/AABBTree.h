/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
#include "BroadPhasePhy.h"
#include <set>
class AABBTree : public Broadphase
{
public:

	AABBTree(float margin = 0.2f);
	~AABBTree(void);
	virtual void HandleAdd(Proxy &proxy);
	virtual void HandleRemove(Proxy &proxy);
	virtual void Update(const TimeStep &timeStep);
	virtual void Pick(const Vec3 &pos, PickResult &result);
	virtual const RayCastResult RayCast(const Ray3 &ray, float maxDistance);
	virtual void QueryRegion(const Proxy &proxy, std::vector<Collider *> &result);
	virtual ColliderPairList &ComputePairs(void);
	virtual void Draw(PhysicsDebugDraw &debugDraw);

private:

	struct Node
	{
		Node(void)
		: parent(nullptr)
		, proxy(nullptr)
		, childrenCrossed(false)
		{
			children[0] = children[1] = nullptr;
		}

		// branch: normal, leaf: fat
		AABB aabb;
		Node *parent;      // branch
		Proxy *proxy;    // leaf
		Node *children[2]; // branch
		bool childrenCrossed;


		void SetBranch(Node *child0, Node *child1)
		{
			if (proxy)
				proxy->userData = nullptr;

			child0->parent = this;
			child1->parent = this;

			children[0] = child0;
			children[1] = child1;
		}

		void SetLeaf(Proxy *p)
		{
			proxy = p;
			proxy->userData = this;
			children[0] = nullptr;
			children[1] = nullptr;
		}

		bool IsLeaf(void) const { return children[0] == nullptr; }
		bool IsBranch(void) const { return !IsLeaf(); }

		void UpdateAABB(float margin)
		{
			if (IsLeaf())
			{
				const Vec3 k_marginVec(margin, margin, margin);
				aabb.minPoint = proxy->aabb.minPoint - k_marginVec;
				aabb.maxPoint = proxy->aabb.maxPoint + k_marginVec;
			}
			else
				aabb = children[0]->aabb.Union(children[1]->aabb);
		}

		Node *GetSibling(void) const
		{
			return
				this == parent->children[0]
				? parent->children[1]
				: parent->children[0];
		}
	};

	void CleanPairs(void);
	ColliderPairList mPairs;

	typedef std::vector<Node *> NodeList;
	void InsertNode(Node *node, Node **parent);
	void RemoveNode(Node *node);
	void ComputePairsHelper(Node *node0, Node *node1);
	void ClearChildrenCrossHelper(Node *node);
	void CrossChildren(Node *node);
	void UpdateNodeHelper(Node *node, NodeList &invalidNodes);


	// DEBUG: check duplicate pairs

	std::set<std::pair<Node *, Node *>> mTraversalRecord;


	Node *mRoot;
	float mMargin;
	FixedSizeAllocator mNodeAllocator;
	NodeList mInvalidNodes;

	Node *AllocateNode(void)
	{
		return new (mNodeAllocator.Allocate()) Node();
	}

	void FreeNode(Node *node)
	{
		mNodeAllocator.Free(node);
	}
};