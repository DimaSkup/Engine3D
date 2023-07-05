#pragma once

#include <d3d11.h>


#include "../Model/ModelMediator.h"
#include "../Model/ModelData.h"
#include "../Model/TextureArrayClass.h"   // for using multiple textures for models
#include "../Model/ModelInitializerInterface.h"  // an interface for model initialization
#include "../Model/ModelInitializer.h"           // a concrete implementation of the ModelInitializerInterface
#include "../Model/VertexBuffer.h"
#include "../Model/IndexBuffer.h"

class Model : public GraphicsComponent
{
public:
	Model();
	virtual ~Model();

	virtual bool InitializeFromFile(ID3D11Device* pDevice, 
		const std::string & modelFilename,
		const std::string & modelID);
	virtual void Render(ID3D11DeviceContext* pDeviceContext);


	const ModelData const* GetModelDataObj() const;
	const TextureArrayClass const* GetTextureArray() const;

private: 
	
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void ShutdownBuffers();

private: 
	static std::string    defaultModelsDirPath_;

	ModelInitializerInterface* pModelInitializer_ = nullptr;
	ModelData*                 pModelData_ = nullptr;        // data object which contains all the model properties
	TextureArrayClass*         pTexturesList_ = nullptr;     // for work with multiple textures
	VertexBuffer<VERTEX>*      pVertexBuffer_ = nullptr;     // for work with a model vertex buffer
	IndexBuffer*               pIndexBuffer_ = nullptr;      // for work with a model index buffer
};