#include "TesselatedPlaneMesh.h"

TesselatedPlaneMesh::TesselatedPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
										 float split, float x, float z, float width, float length)
	: split(split), x(x), z(z), width(width), length(length)
{
	initBuffers(device);
}

TesselatedPlaneMesh::~TesselatedPlaneMesh() { BaseMesh::~BaseMesh(); }

void TesselatedPlaneMesh::initBuffers(ID3D11Device* device)
{
	vBuffers.resize(split * split);
	iBuffers.resize(split * split);

	if (split == 1.f)
	{
		VertexType* vertices;
		uint32_t* indices;
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;

		vertexCount = 4;
		indexCount = 4;

		vertices = new VertexType[vertexCount];
		indices = new uint32_t[indexCount];

		vertices[0].position = XMFLOAT3(x, 0.0f, z + length);		 // top left
		vertices[1].position = XMFLOAT3(x, 0.f, z);					 // bottom left
		vertices[2].position = XMFLOAT3(x + width, 0.f, z);			 // bottom right
		vertices[3].position = XMFLOAT3(x + width, 0.f, z + length); // top right

		vertices[0].texture = XMFLOAT2(0.f, 0.f);
		vertices[1].texture = XMFLOAT2(0.f, 1.f);
		vertices[2].texture = XMFLOAT2(1.f, 1.f);
		vertices[3].texture = XMFLOAT2(1.f, 0.f);

		vertices[0].normal = XMFLOAT3(0.f, 1.f, 0.f);
		vertices[1].normal = XMFLOAT3(0.f, 1.f, 0.f);
		vertices[2].normal = XMFLOAT3(0.f, 1.f, 0.f);
		vertices[3].normal = XMFLOAT3(0.f, 1.f, 0.f);

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 3;

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		device->CreateBuffer(&vertexBufferDesc, &vertexData, &vBuffers[0]);

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;
		device->CreateBuffer(&indexBufferDesc, &indexData, &iBuffers[0]);

		delete[] vertices;
		vertices = nullptr;
		delete[] indices;
		indices = nullptr;
		return;
	}

	for (int i = split - 1; i >= 0; i--)
	{
		for (int j = split - 1; j >= 0; j--)
		{
			VertexType* vertices;
			uint32_t* indices;
			D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData, indexData;

			vertexCount = 4;
			indexCount = 4;

			vertices = new VertexType[vertexCount];
			indices = new uint32_t[indexCount];

			float xPos = x + i * width / split;
			float nextXPos = x + (i + 1) * width / split;

			float zPos = z + (split - 1 - j) * length / split;
			float nextZPos = z + (split - j - 2) * length / split;

			vertices[0].position = XMFLOAT3(xPos, 0.0f, zPos);		 // top left
			vertices[1].position = XMFLOAT3(xPos, 0.f, nextZPos);					 // bottom left
			vertices[2].position = XMFLOAT3(nextXPos, 0.f, nextZPos);			 // bottom right
			vertices[3].position = XMFLOAT3(nextXPos, 0.f, zPos); // top right

			float u = i / split;
			float v = (split - j) / split;
			float u1 = (i + 1) / split;
			float v1 = (split - j - 1) / split;
			vertices[0].texture = XMFLOAT2(u, v);
			vertices[1].texture = XMFLOAT2(u, v1);
			vertices[2].texture = XMFLOAT2(u1, v1);
			vertices[3].texture = XMFLOAT2(u1, v);

			vertices[0].normal = XMFLOAT3(0.f, 1.f, 0.f);
			vertices[1].normal = XMFLOAT3(0.f, 1.f, 0.f);
			vertices[2].normal = XMFLOAT3(0.f, 1.f, 0.f);
			vertices[3].normal = XMFLOAT3(0.f, 1.f, 0.f);

			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 3;

			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			device->CreateBuffer(&vertexBufferDesc, &vertexData, &vBuffers[i * split + j]);

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			indexData.pSysMem = indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;
			device->CreateBuffer(&indexBufferDesc, &indexData, &iBuffers[i * split + j]);

			delete[] vertices;
			vertices = nullptr;
			delete[] indices;
			indices = nullptr;
		}
	}
}

void TesselatedPlaneMesh::sendData(ID3D11DeviceContext* deviceContext,uint32_t splitIndex, D3D_PRIMITIVE_TOPOLOGY top)
{
	uint32_t stride = sizeof(VertexType);
	uint32_t offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vBuffers[splitIndex], &stride, &offset);
	deviceContext->IASetIndexBuffer(iBuffers[splitIndex], DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
