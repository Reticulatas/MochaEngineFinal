%{
#include "PhysicsEngine.h"
#include "MathIncludes.h"
%}

struct RayCastResult
{
public:
	bool hit;
	float t;
	Math::Vector3 intersection;
	Math::Vector3 normal;
	Collider *collider;
	GameObject* go;
private:
	RayCastResult();
	~RayCastResult();
};

class PhysicEngine
{
	~PhysicsEngine();
	PhysicsEngine();
public:
	GameObject* DragObject(Point3 CamPos, Point3 Mouse);
	GameObject* DragObject_DoRaycast(Point3 CamPos, Point3 Mouse, GameObject* obj, float);
	GameObject* RayCasting(Point3 start, Point3 end, Color color);
	GameObject* RayCast(Point3 start, Vector vec, Color color);
	GameObject* RayCast3D(const Math::Vector3 &pos, const Math::Vector3 &dir, float maxDistance);
	GameObject* RayCast3D(const Math::Vector3& pos, const Math::Vector3& dir, CCamera* camera, int screen_x, int screen_y, float maxDist);
	GameObject* RayCast3DWithTag(const Math::Vector3& pos, const Math::Vector3& dir, CCamera* camera, int screen_x, int screen_y, float maxDist, std::string tag);
	GameObject* RayCast3DWithTag(const Math::Vector3 &pos, const Math::Vector3 &dir, float maxDistance, std::string tag);
	
	RayCastResult RayCast3D_Output(const Math::Vector3 &pos, const Math::Vector3 &dir, float maxDistance);

	RayCastResult RayCast3DWithTag_Output(const Math::Vector3 &pos, const Math::Vector3 &dir, float maxDistance, std::string& tag);

	 
	bool IsActive() const;
	void IsActive(bool val);

	float GetTimeStep(void) const;
	const Vector &GetGravity(void) const;
	//PhysicEngine &SetGravity(const Vector &g);

	static PhysicEngine* getInstance();
};
