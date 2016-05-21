/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#pragma once

#include "Asset.h"
#include "MathIncludes.h" 
#include <vector>
#include "MatAttribNode.h"
#include "AssetManager.h"
#include "AssetHandle.h"
#include "Shader.h"
#include "ShaderNode.h" 
#include "MaterialImpl.h"
#include "RenderTarget.h"

class Shader;
 
class Material : public Asset 
{
//-----------------------------------------------------------------------//
/* /////////////////////// MEMBER VARIABLES /////////////////////////////*/
//-----------------------------------------------------------------------//
	
//-----------------------------------------------------------------------//
/* /////////////////////// PUBLIC METHODS /////////////////////////////*/
//-----------------------------------------------------------------------//
public:
	MaterialImpl mMaterialData;

  // Constructors & Destructor
  Material();
  Material(const std::string& name) : Asset() { }
  Material(const Material& c);
  virtual ~Material();

  // Shader Operations
  void SetShader(const char* shaderName);
  Shader* GetShader();

  // Attribute operations
  void BindAttributes(Shader* overrideShader = 0);
  MatAttribNode* GetAttribute(unsigned type_id, unsigned index = 0);
  template <typename T>
  T* GetAttribute(unsigned index = 0) {
	  static_assert(std::is_base_of<MatAttribNode, T>::value, "Material Attribute to Get must be derived from MatAttribNode");
	  return mMaterialData.GetAttribute<T>(index);
  }
  void RebuildMaterial();		/// Rebuild Material from Shader
  void Clear();
  void OnInit();
  // Quick Texture Access
  virtual RenderTarget* GetTexture();
  virtual void SetTexture(const std::string& textureName);
  virtual void SetTexture(const AssetHandle& assetHandle);
  virtual void SetTexture(unsigned textureId){}	
  void SetTextureOffset(const Vector2& uvOffset, int textureIndex = 0);
  void SetTextureScale(const Vector2& uvScale, int textureIndex = 0);
  unsigned GetTextureWidth(unsigned textureIndex);
  unsigned GetTextureHeight(unsigned textureIndex);
  float GetColorAlpha();
  void SetColorAlpha(float alpha);
  void SetColor(const Color& col);
  void SetColor(float r, float g, float b, float a = 1.0f);
  Color GetColor();

  // Operators
  virtual Material& operator=(const Material& rhs);

  virtual Material* Clone();

	___SERIALIZE_SAVE_BEGIN___
		___DEFBASE(Asset);
		___DEFSER(mMaterialData, 2)
	___SERIALIZE_END___
	___SERIALIZE_LOAD_BEGIN___
		___DEFBASE(Asset);
		___DEFSER(mMaterialData, 2)
		OnInit();
	___SERIALIZE_END___
		___SERIALIZE_SAVE_LOAD___


	friend class CMeshRenderer;
	friend class MaterialComponent; 
};
 
___SERIALIZE_CLASS(Material, 2);

/////////////////////////////////////
//Original Author: Eduardo Chaves
//Modifications:
//	Date	-	Change