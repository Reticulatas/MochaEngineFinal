%module SharpMocha

/*----sys headers----*/

//macros
%define %cs_callback(TYPE, CSTYPE)
%typemap(ctype) TYPE, TYPE& "void*"
%typemap(in) TYPE  %{ $1 = ($1_type)$input; %}
%typemap(in) TYPE& %{ $1 = ($1_type)&$input; %}
%typemap(imtype, out = "IntPtr") TYPE, TYPE& "CSTYPE"
%typemap(cstype, out = "IntPtr") TYPE, TYPE& "CSTYPE"
%typemap(csin) TYPE, TYPE& "$csinput"
%enddef

// typemaps to marshal const unsigned char * as byte[]k
/*%typemap(ctype) const unsigned char *buffer "const unsigned char *"
%typemap(imtype) const unsigned char *buffer "byte[]"
%typemap(cstype) const unsigned char *buffer "byte[]"
%typemap(in) const unsigned char *buffer %{ $1 = ($1_ltype)$input; % }
%typemap(csin) const unsigned char *buffer "$csinput"*/

//system headers
%include "std_string.i"
%include "std_vector.i"
%include "std_set.i"
%include "std_list.i"
%include "std_map.i"



//generic vectors
%template(IntVector)std::vector<int>;

%cs_callback(MochaDotNetCallback, NativeMochaDotNetCallback)
%cs_callback(MochaDotNetCScriptCopyCallback, NativeMochaDotNetCScriptCopyCallback)
%cs_callback(MochaDotNetCScriptEventCallback, NativeMochaDotNetCScriptEventCallback)
%cs_callback(MochaDotNetCScriptCallback, NativeMochaDotNetCScriptCallback)

/*-----user headers----*/

%typemap(csimports) SWIGTYPE %{
	using System;
	using System.Runtime.InteropServices;

	//using MochaDotNET;
	//using System.ComponentModel;
%}

//utils and math
%include "Vector3.i"
%include "Color.i"
%include "EulerAngles.i"
%include "Vertex.i"

%{
#include "MathIncludes.h"
%}

typedef Math::Vector4 Vector;
typedef Math::Vector4 Point4;
typedef Math::Vector4 Point3;
typedef Math::Vector2 MPoint2;
typedef Math::Matrix4 Matrix;
typedef Math::Vector3 Vec3;
typedef Math::Matrix3 Mat3;

//make all classes past this point extendable
%typemap(csclassmodifiers) SWIGTYPE "public partial class"
/*%typemap(csinterfaces) SWIGTYPE "MochaSwigBase, INotifyPropertyChanged"
%typemap(csvarin, excode = SWIGEXCODE2) SWIGTYPE, SWIGTYPE *, SWIGTYPE &, SWIGTYPE [], SWIGTYPE (CLASS::*), enum SWIGTYPE, SWIGTYPE *const,
 SWIGTYPE *const& %{ 
	set{
		if (Engine.getInstance().GetIsEditorMode())
		{
			string s = "$imcall";
			s = s.Remove(s.LastIndexOf('_'));
			s = s.Substring(s.LastIndexOf('_'));
			OnPropertyChanged(s);
		}
		$imcall; $excode
	}
%}
*/

//%typemap(cscode) SWIGTYPE %{
//%}

%typemap(cscode) IMeta 
%{
	public global::System.Runtime.InteropServices.HandleRef get_CPtr() {
		return swigCPtr;
	}

	public static global::System.Runtime.InteropServices.HandleRef get_static_CPtr(IMeta obj) {
		return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
	}
	
	public IMeta m_castedSource;

	public bool IsKindOf<T>() where T : IMeta
	{
		// Use your C++ RTTI system to test if this wrapped C# object's
		// native object is a 'typeof(T).Name'
		return (typeof(T).Name == this.mytoplevelname());
	}

	public override bool Equals(object o) {
		if (!(o is IMeta))
			return false;
		return this.swigCPtr.Handle == (($csclassname)o).swigCPtr.Handle;
	}

	public override int GetHashCode()
	{
		return this.swigCPtr.Handle.ToInt32();
	}
	//public event PropertyChangedEventHandler PropertyChanged;
%}

//engine
%include "IMeta.i"
%include "LayerMask.i"
%include "GameObject.i"
%include "SlotTable.i"
%include "Engine.i"
%include "MochaWindow.i"
%include "AssetManager.i"
%include "BaseState.i"
%include "ObjectFactory.i"
%include "GSM.i"
%include "Map.i"
%include "Barrista.i"
%include "ScriptSubsystem.i"
%include "EditorSubsystem.i"
%include "Material.i"
%include "FrameController.i"
%include "Input.i"
%include "PhysicsEngine.i"
%include "Animation.i"
%include "Sound.i"
%include "Oculus.i"
%include "Renderer.i"
%include "Prefab.i"
%include "AudioManager.i"

//components
%include "Component.i"
%include "CTransform.i"
%include "CRenderer.i"
%include "CMeshRenderer.i"
%include "CSkinMeshRenderer.i"
%include "CAnimationController.i"
%include "CLight.i"
%include "CPhysics.i"
%include "CCollider.i"
%include "CCamera.i"
%include "CSound.i"
%include "IZone.i"
%include "CZone.i"
%include "CMapZone.i"
%include "CStreamZone.i"
%include "CScript.i"
%include "CDiffuseRenderer.i"
%include "CForwardRenderer.i"
%include "CFontRenderer.i"
%include "CReflectionProbe.i"
%include "CConvexHull.i"
%include "CParticleEmitter.i"
%include "CParticleRenderer.i"
%include "CPostProcess.i"
%include "CBloom.i"
%include "CMotionBlur.i"
%include "CCensorFX.i"
%include "CFilmGrain.i"
%include "CChromaticAberration.i"
%include "CArtifactFX.i"
%include "CShadowProbe.i"