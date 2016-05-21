#include "stdafx.h"
#include "AABBTree.h"
#include "SupportFunction.h"

AABBTree::AABBTree(float margin)
: mRoot(nullptr)
, mMargin(margin)
, mNodeAllocator(sizeof(Node))
{

}

AABBTree::~AABBTree(void)
{
	CleanPairs();
}

void AABBTree::HandleAdd(Proxy &proxy)
{
	if (mRoot)
	{
		// not first node, insert node to tree
		Node *node = AllocateNode();
		node->SetLeaf(&proxy);
		node->UpdateAABB(mMargin);
		InsertNode(node, &mRoot);
	}
	else
	{
		// first node, make root
		mRoot = AllocateNode();
		mRoot->SetLeaf(&proxy);
		mRoot->UpdateAABB(mMargin);
	}
}

void AABBTree::InsertNode(Node *node, Node **parent)
{
	Node *p = *parent;
	if (p->IsLeaf())
	{
		// parent is leaf, simply split
		Node *newParent = AllocateNode();
		newParent->parent = p->parent;
		newParent->SetBranch(node, p);
		*parent = newParent;
	}
	else
	{
		// parent is branch, compute volume differences between pre-merge and post-merge
		const AABB &aabb0 = p->children[0]->aabb;
		const AABB &aabb1 = p->children[1]->aabb;
		const float volumeDiff0 = aabb0.Union(node->aabb).Volume() - aabb0.Volume();
		const float volumeDiff1 = aabb1.Union(node->aabb).Volume() - aabb1.Volume();

		// insert to the child that produces less volume increase
		if (volumeDiff0 < volumeDiff1)
			InsertNode(node, &p->children[0]);
		else
			InsertNode(node, &p->children[1]);
	}

	// update parent AABB (propagates back up the recursion stack)
	(*parent)->UpdateAABB(mMargin);
}

void AABBTree::RemoveNode(Node *node)
{
	GV_ASSERT(mRoot);

	// replace parent with sibling, remove parent node
	Node *parent = node->parent;
	if (parent) // node is not root
	{
		Node *sibling = node->GetSibling();
		if (parent->parent) // if there's a grandparent
		{
			// update links
			sibling->parent = parent->parent;
			(
				parent == parent->parent->children[0]
				? parent->parent->children[0]
				: parent->parent->children[1]
				) = sibling;
		}
		else // no grandparent
		{
			// make sibling root
			Node *sibling = node->GetSibling();
			mRoot = sibling;
			sibling->parent = nullptr;
		}
		FreeNode(node);
		FreeNode(parent);
	}
	else // node is root
	{
		FreeNode(node);
		mRoot = nullptr;
	}
}

void AABBTree::HandleRemove(Proxy &proxy)
{
	Node *node = static_cast<Node *>(proxy.userData);
	GV_ASSERT(node);
	proxy.userData = nullptr;
	RemoveNode(node);
}

void AABBTree::Update(const TimeStep &timeStep)
{
	if (mRoot)
	{
		if (mRoot->IsLeaf())
			mRoot->UpdateAABB(mMargin);
		else
		{
			// grab all invalid nodes
			mInvalidNodes.clear();
			UpdateNodeHelper(mRoot, mInvalidNodes);

			// re-insert all invalid nodes
			for (Node *node : mInvalidNodes)
			{
				// grab parent link (pointer to the pointer that points to parent)
				Node *parent = node->parent;
				Node *sibling = node->GetSibling();
				Node **parentLink =
					parent->parent
					? (parent == parent->parent->children[0] ? &parent->parent->children[0] : &parent->parent->children[1])
					: &mRoot;

				// replace parent with sibling
				sibling->parent = parent->parent ? parent->parent : nullptr;
				*parentLink = sibling;
				FreeNode(parent);

				// re-insert node
				node->UpdateAABB(mMargin);
				InsertNode(node, &mRoot);
			}
			mInvalidNodes.clear();
		}

	}
}

void AABBTree::UpdateNodeHelper(Node *node, NodeList &invalidNodes)
{
	if (node->IsLeaf())
	{
		// check if fat AABB doesn't contain proxy's AABB anymore
		if (!node->aabb.Contains(node->proxy->aabb))
			invalidNodes.push_back(node);
	}
	else
	{
		UpdateNodeHelper(node->children[0], invalidNodes);
		UpdateNodeHelper(node->children[1], invalidNodes);
	}
}

void AABBTree::Pick(const Vec3 &pos, PickResult &result)
{
	std::queue<Node *> q;
	if (mRoot)
		q.push(mRoot);

	while (!q.empty())
	{
		Node *n = q.front();
		q.pop();

		if (n->IsLeaf())
		{
			if (n->proxy->parent->CollidesCollider(pos))
				result.push_back(n->proxy->parent);
		}
		else
		{
			q.push(n->children[0]);
			q.push(n->children[1]);
		}
	}
}

const RayCastResult AABBTree::RayCast(const Ray3 &ray, float maxDistance)
{
	RayCastResult result;
	result.hit = false;
	result.t = FLT_MAX;

	std::queue<Node *> q;
	if (mRoot)
		q.push(mRoot);
	else
		return result;

	while (!q.empty())
	{
		Node &node = *q.front();
		q.pop();

		const bool isLeaf = node.IsLeaf();
		AABB &aabb = isLeaf ? node.proxy->aabb : node.aabb;

		float t;
		if (RayAABBWithTime(ray, aabb, maxDistance, t, !isLeaf))
		{
			// the node cannot possibly give closer results, skip
			//if (result.hit && result.t < t)
			//  continue;
			// ^^^^^^^^^^^^^^^ TODO: buggy, will fix later

			if (isLeaf)
			{
				Collider &collider = *node.proxy->parent;

				// skip sensors
				if (collider.IsSensor())
					continue;

				Vec3 n;
				float t;
				if (collider.RayCastCollider(ray, maxDistance, t, n))
				{
					result.hit = true;
					result.collider = &collider;
					result.t = t;
					result.normal = n;
				}
			}
			else // is branch
			{
				q.push(node.children[0]);
				q.push(node.children[1]);
			}
		}
	}

	if (result.hit)
	{
		result.ray = ray;
		result.intersection = ray.pos + result.t * maxDistance * ray.dir.Normalized();
	}

	return result;
}

void AABBTree::QueryRegion(const Proxy &proxy, std::vector<Collider *> &result)
{
	std::queue<Node *> q;
	if (mRoot)
		q.push(mRoot);

	while (!q.empty())
	{
		Node *n = q.front();
		q.pop();

		if (n->IsLeaf())
		{
			if (proxy.Collides(*n->proxy))
				result.push_back(n->proxy->parent);
		}
		else
		{
			q.push(n->children[0]);
			q.push(n->children[1]);
		}
	}
}

void AABBTree::CleanPairs(void)
{
	while (!mPairs.empty())
		FreePair(mPairs.pop_front());
}

ColliderPairList &AABBTree::ComputePairs(void)
{
	CleanPairs();

	// early out
	if (!mRoot || mRoot->IsLeaf())
		return mPairs;

	// clear Node::childrenCrossed flags
	if (mRoot)
		ClearChildrenCrossHelper(mRoot);

	// DEBUG: check duplicate pairs

	mTraversalRecord.clear();


	ComputePairsHelper(mRoot->children[0], mRoot->children[1]);

	return mPairs;
}

void AABBTree::ClearChildrenCrossHelper(Node *node)
{
	node->childrenCrossed = false;
	if (node->IsBranch())
	{
		ClearChildrenCrossHelper(node->children[0]);
		ClearChildrenCrossHelper(node->children[1]);
	}
}

void AABBTree::CrossChildren(Node *node)
{
	if (!node->childrenCrossed)
	{
		ComputePairsHelper(node->children[0], node->children[1]);
		node->childrenCrossed = true;
	}
}

void AABBTree::ComputePairsHelper(Node *node0, Node *node1)
{
	// early out
	if (!node0->aabb.Collides(node1->aabb))
		return;

	if (node0->IsLeaf())
	{
		// 2 leaves, check proxies instead of fat AABBs
		if (node1->IsLeaf())
		{
			if (node0->proxy->Collides(*node1->proxy))
				mPairs.push_back(AllocatePair(node0->proxy->parent, node1->proxy->parent));

			// DEBUG: check duplicate pairs

			auto pair = std::make_pair(node0, node1);
			GV_ASSERT(mTraversalRecord.find(pair) == mTraversalRecord.end());
			mTraversalRecord.insert(pair);

		}
		// 1 branch / 1 leaf, 2 cross checks
		else
		{
			CrossChildren(node1);
			if (node0->aabb.Collides(node1->aabb))
			{
				ComputePairsHelper(node0, node1->children[0]);
				ComputePairsHelper(node0, node1->children[1]);
			}
		} // end of if (node1->IsLeaf()) else
	} // end of if (node0->IsLeaf())
	else
	{
		// 1 branch / 1 leaf, 2 cross checks
		if (node1->IsLeaf())
		{
			CrossChildren(node0);
			if (node0->aabb.Collides(node1->aabb))
			{
				ComputePairsHelper(node0->children[0], node1);
				ComputePairsHelper(node0->children[1], node1);
			}
		}
		// 2 branches, 4 cross checks
		else
		{
			CrossChildren(node0);
			CrossChildren(node1);
			if (node0->aabb.Collides(node1->aabb))
			{
				ComputePairsHelper(node0->children[0], node1->children[0]);
				ComputePairsHelper(node0->children[0], node1->children[1]);
				ComputePairsHelper(node0->children[1], node1->children[0]);
				ComputePairsHelper(node0->children[1], node1->children[1]);
			}
		} // end of if (node1->IsLeaf()) else
	} // end of if (node0->IsLeaf()) else
}

void AABBTree::Draw(PhysicsDebugDraw &debugDraw)
{}