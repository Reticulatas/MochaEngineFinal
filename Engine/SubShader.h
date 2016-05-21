/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */

#pragma once

#include "GraphicsInclude.h"
#include "boost/filesystem/path.hpp"
#include "Log.h"

const std::string VSfilstr = "vs";
const std::string PSfilstr = "ps";
const std::string CSfilstr = "cs";
const std::string GSfilstr = "gs";

enum SUBSHADERTYPE
{
  VERTEX_SHADER		= 1,
  PIXEL_SHADER		= 2,
  COMPUTE_SHADER	= 4,
  GEOMETRY_SHADER	= 8,
  UNASSIGNED_SHADER = 16
};

template<typename T>
class SubShader : public Asset
{
  SUBSHADERTYPE mSubShaderType;
  std::wstring mSubShaderName;
  T* mSubShader;
  ID3DBlob* mShaderBuffer;
  
  void LoadFromFile(std::wstring const Filename);			/* Load and compile shader from file */
  void OutputShaderErrorMessage(ID3D10Blob* errorMessage, const WCHAR* shaderFilename);

  HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

  // Condition compilation of different types of shaders
  template <typename Y>
  void LoadShaderFromFile(std::wstring const Filename, typename std::enable_if<std::is_same<Y, ID3D11VertexShader>::value>::type* = 0)
  {

	  ID3D11Device*	device = Renderer::getInstance()->GetD3D11Device();
	  ID3D10Blob* err = 0;
	  ID3DBlob* tempShaderBuffer = 0;
	  HRESULT hr;

	  // Compile pixel shader shader
	  hr = D3DX11CompileFromFile(Filename.c_str(), 0, 0, "VS", "vs_5_0", D3DCOMPILE_DEBUG, 0, 0, &tempShaderBuffer, &err, 0);
	  if (FAILED(hr))
	  {
		  if (tempShaderBuffer)
			  tempShaderBuffer->Release();

		  if (err)
			  OutputShaderErrorMessage(err, Filename.c_str());
	  }
	  else
	  {
		  if (mShaderBuffer)
			  Release();
		  mSubShaderName = Filename;
		  mShaderBuffer = tempShaderBuffer;
		  device->CreateVertexShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &mSubShader);
	  }
  }
 
  template <typename Y>
  void LoadShaderFromFile(std::wstring const Filename, typename std::enable_if<std::is_same<Y, ID3D11PixelShader>::value>::type* = 0)
  {
    ID3D11Device*	device = Renderer::getInstance()->GetD3D11Device();
    ID3D10Blob* err = 0;
	ID3DBlob* tempShaderBuffer = 0;
    HRESULT hr;

    // Compile pixel shader shader
	hr = D3DX11CompileFromFile(Filename.c_str(), 0, 0, "PS", "ps_5_0", D3DCOMPILE_DEBUG, 0, 0, &tempShaderBuffer, &err, 0);
    if (FAILED(hr))
    {
		if (tempShaderBuffer)
			tempShaderBuffer->Release();

      if (err)
        OutputShaderErrorMessage(err, Filename.c_str());
    }
    else
    {
		if (mShaderBuffer)
			Release();
		mSubShaderName = Filename;
		mShaderBuffer = tempShaderBuffer;
		device->CreatePixelShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &mSubShader);
    }
  }

  template <typename Y>
  void LoadShaderFromFile(std::wstring const Filename, typename std::enable_if<std::is_same<Y, ID3D11ComputeShader>::value>::type* = 0)
  {
	 ID3D11Device*	device = Renderer::getInstance()->GetD3D11Device();
    ID3D10Blob* err = 0;
	ID3DBlob* tempShaderBuffer = 0;
    HRESULT hr;

    // Compile pixel shader shader
	hr = D3DX11CompileFromFile(Filename.c_str(), 0, 0, "CS", "cs_5_0", D3DCOMPILE_DEBUG, 0, 0, &tempShaderBuffer, &err, 0);
    if (FAILED(hr))
    {
		if (tempShaderBuffer)
			tempShaderBuffer->Release();

      if (err)
        OutputShaderErrorMessage(err, Filename.c_str());
    }
    else
    {
		if (mShaderBuffer)
			Release();
		mSubShaderName = Filename;
		mShaderBuffer = tempShaderBuffer;
		device->CreateComputeShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &mSubShader);
    }
  }
  template <typename Y>
  void LoadShaderFromFile(std::wstring const Filename, typename std::enable_if<std::is_same<Y, ID3D11GeometryShader>::value>::type* = 0)
  {

	  ID3D11Device*	device = Renderer::getInstance()->GetD3D11Device();
	  ID3D10Blob* err = 0;
	  ID3DBlob* tempShaderBuffer = 0;
	  HRESULT hr;

	  // Compile pixel shader shader
	  hr = D3DX11CompileFromFile(Filename.c_str(), 0, 0, "GS", "gs_5_0", D3DCOMPILE_DEBUG, 0, 0, &tempShaderBuffer, &err, 0);
	  if (FAILED(hr))
	  {
		  if (tempShaderBuffer)
			  tempShaderBuffer->Release();

		  if (err)
			  OutputShaderErrorMessage(err, Filename.c_str());
	  }
	  else
	  {
		  if (mShaderBuffer)
			  Release();
		  mSubShaderName = Filename;
		  mShaderBuffer = tempShaderBuffer;
		  device->CreateGeometryShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &mSubShader);
	  }
  }


public:

  SubShader();
  SubShader(std::wstring const Filename, SUBSHADERTYPE sstype);
  ~SubShader();

  SUBSHADERTYPE SubShaderType() const { return mSubShaderType; }
  T* GetSubShader();
  ID3DBlob* GetShaderBuffer();
  void Release();

  virtual void OnAssetFileChanged();

  metadef(SubShader)
  m_addbase(Asset)
  endmetadef;
};

template<typename T>
void SubShader<T>::Release()
{
  mShaderBuffer->Release();
  mSubShader->Release();
  mShaderBuffer = 0;
  mSubShader = 0;
}

template<typename T>
void SubShader<T>::OnAssetFileChanged()
{
  LoadFromFile(mSubShaderName);
  Log(std::string("Reloaded ") + std::string(mSubShaderName.begin(), mSubShaderName.end()));
}

template<typename T>
SubShader<T>::SubShader() : Asset()
{
  mSubShader = UNASSIGNED_SHADER;
}

template<typename T>
SubShader<T>::SubShader(std::wstring const Filename, SUBSHADERTYPE sstype) : Asset()
{
  mSubShaderType = sstype;

  LoadFromFile(Filename);
}

template<typename T>
SubShader<T>::~SubShader()
{
  Release();
}

template<typename T>
ID3DBlob* SubShader<T>::GetShaderBuffer()
{
  return mShaderBuffer;
}

template<typename T>
T* SubShader<T>::GetSubShader()
{
  return mSubShader;
}

template<typename T>
void SubShader<T>::LoadFromFile(std::wstring const Filename)
{
  DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
  shaderFlags |= D3D10_SHADER_DEBUG;
  shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
  shaderFlags |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
#endif
  shaderFlags = 0;
  
  LoadShaderFromFile<T>(Filename);  
}

template<typename T>
HRESULT SubShader<T>::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
  if (!srcFile || !entryPoint || !profile || !blob)
    return E_INVALIDARG;

  *blob = nullptr;

  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
  flags |= D3DCOMPILE_DEBUG;
#endif

  ID3DBlob* shaderBlob = nullptr;
  ID3DBlob* errorBlob = nullptr;

  HRESULT hr = D3DCompileFromFile(srcFile, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE,
    entryPoint, profile,
    flags, 0, &shaderBlob, &errorBlob);
  if (FAILED(hr))
  {
    if (errorBlob)
    {
      Log((char*)errorBlob->GetBufferPointer(), LogEntry::Error);
      errorBlob->Release();
    }

    if (shaderBlob)
      shaderBlob->Release();

    return hr;
  }

  *blob = shaderBlob;

  return hr;
}

template<typename T>
void SubShader<T>::OutputShaderErrorMessage(ID3D10Blob* errorMessage, const WCHAR* shaderFilename)
{
  char* compileErrors;
  unsigned long bufferSize, i;
  ofstream fout;

  // Get a pointer to the error message text buffer.
  compileErrors = (char*)(errorMessage->GetBufferPointer());

  Log(compileErrors, LogEntry::Error);
  
  // Get the length of the message.
  bufferSize = errorMessage->GetBufferSize();

  // Open a file to write the error message to.
  fout.open("shader-error.txt");

  // Write out the error message.
  for (i = 0; i < bufferSize; i++)
  {
    fout << compileErrors[i];
  }

  // Close the file.
  fout.close();

  // Release the error message.
  errorMessage->Release();
  errorMessage = 0;

  return;
}
/////////////////////////////////////
//Original Author: Kenneth Tan
//Modifications:
//	Date	-	Change