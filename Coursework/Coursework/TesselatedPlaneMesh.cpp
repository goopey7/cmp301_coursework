#include "TesselatedPlaneMesh.h"

TesselatedPlaneMesh::TesselatedPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
										 float x, float z, float width, float length)
	: x(x), z(z), width(width), length(length)
{
	initBuffers(device);
}

TesselatedPlaneMesh::~TesselatedPlaneMesh() { BaseMesh::~BaseMesh(); }

void TesselatedPlaneMesh::initBuffers(ID3D11Device* device)
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

	indices[0] = 0; // top left
	indices[1] = 1; // bottom left
	indices[2] = 2; // bottom right
	indices[3] = 3; // top right

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;
}

void TesselatedPlaneMesh::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	uint32_t stride = sizeof(VertexType);
	uint32_t offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
