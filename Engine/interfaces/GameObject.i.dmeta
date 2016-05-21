
%{
#include "GameObject.h"
	%}

namespace std {
	%template(ComponentVector) vector<Component*>;
}

%typemap(cscode) GameObject %{
	public global::System.Runtime.InteropServices.HandleRef get_CPtr() {
		return swigCPtr;
	}

	public bool get_swigMemOwn()
	{
		return swigCMemOwn;
	}

	public GameObject(global::System.IntPtr cPtr) {
		swigCMemOwn = false;
		swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
	}

	public T RequireComponent<T>() where T : Component{
		Component from = RequireComponentByName(typeof(T).Name);
		System.Reflection.MethodInfo CPtrGetter = from.GetType().GetMethod("getCPtr", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
		return CPtrGetter == null ? default(T) : (T)System.Activator.CreateInstance
			(
			typeof(T),
			System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance,
			null,
			new object[] { ((HandleRef)CPtrGetter.Invoke(null, new object[] { from })).Handle, true},
			null
			);
	}

	public T GetComponentInChildren<T>() where T : Component{
		Component from = GetComponentInChildren(typeof(T).Name);
		if (from == null)
			return null;
		System.Reflection.MethodInfo CPtrGetter = from.GetType().GetMethod("getCPtr", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
		return CPtrGetter == null ? default(T) : (T)System.Activator.CreateInstance
			(
			typeof(T),
			System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance,
			null,
			new object[] { ((HandleRef)CPtrGetter.Invoke(null, new object[] { from })).Handle, true},
			null
			);
	}

	public T GetComponent<T>() where T : Component {
		Component from = GetComponentByName(typeof(T).Name, false);
		if (from == null)
			return null;
		System.Reflection.MethodInfo CPtrGetter = from.GetType().GetMethod("getCPtr", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static);
		return CPtrGetter == null ? default(T) : (T)System.Activator.CreateInstance
			(
			typeof(T),
			System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance,
			null,
			new object[] { ((HandleRef)CPtrGetter.Invoke(null, new object[] { from })).Handle, true},
			null
			);
	}

		public static bool operator==(GameObject g1, GameObject g2)
	{
		if (System.Object.ReferenceEquals(g1, g2))
			return true;
		if ((object)g1 == null || (object)g2 == null)
			return false;
		return g1.GetID() == g2.GetID();
	}
	public static bool operator!=(GameObject g1, GameObject g2)
	{
		return !(g1 == g2);
	}
	public override bool Equals(object obj)
	{
		if (!(obj is GameObject))
			return false;
		return ((obj as GameObject) == this);
	}
	public override int GetHashCode()
	{
		return GetID().GetHashCode();
	}

	public GameObject() { throw new InvalidOperationException("Cannot instantiate game object directly - use ObjectFactory"); }

%}

%template(TagSet) std::set<string>; 

class GameObject
{	
	GameObject();
	~GameObject();

	
public:

	std::set<std::string> flags;

	GameObject* Duplicate();
	void Destroy();

	void SetFlag(std::string f);
	void ClearFlag(std::string f);
	bool GetFlag(std::string f);
	unsigned GetID();

	bool HasComponentByName(std::string name);

	std::string GetName(void) const;
	std::string SetName(std::string s);

	Component* RequireComponentByName(std::string name);
	Component* GetComponentByName(std::string name);
	Component* GetComponentByName(std::string name, bool logerror);
	Component* GetComponentInChildren(std::string name);

	CTransform* transform;
	std::string mName;
	std::string mGUID;
	std::vector<Component*> mComponents;	
	bool mIsSelected;

	LayerMask mLayerMask;

	void LookAt(const Vector3& point);
	void LookAt(const GameObject& point);
	bool GetIsDormantObject(void);


	Prefab* linkedPrefab;
	GameObject* RevertToPrefab();
	void WriteToPrefab();
	void DetachFromPrefab();

	static GameObject* GetGameObjectByName(std::string name_, bool errorIfNotFound = true);
	static std::vector<GameObject*> GetGameObjectsWithTag(std::string tag);
	static GameObject* GetGameObjectWithTag(std::string tag);
};