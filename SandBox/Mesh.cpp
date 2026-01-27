#include <Mesh.h>

Mesh::Mesh()
{
	m_Node.SetIdentity();
}

Mesh::~Mesh()
{
}

void Mesh::Update(F32 dTime)
{
	// Rotate 180° per sec
	static float angle = 0;
	float angle_inc = dTime*2.f/6.28f;
	angle += angle_inc;
//	*reinterpret_cast<XMMATRIX*>(&m_Node) = XMMatrixRotationY(angle);
}

void Mesh::Draw()
{
	auto* HAL = gData.Rdr->GetHAL();

	if (m_Constant == nullptr)
	{
		m_Constant = HAL->CreateConstantBuffer(sizeof(InstanceConstant));
		InstanceConstant* c;
		m_Constant->Lock(0, 0, (void**)&c);
		c->WorldMatrix = m_Node;
		m_Constant->Unlock();
	}

	HAL->BindGraphicPipelineState(ShaderMap::BaseMeshVS, ShaderMap::BasePassPS);
	HAL->SetConstantBuffer(1, SHADER_TYPE_VERTEX, m_Constant);

	U32 SubSetCount = SubSetsDA.GetSize();
	for(int i=0;i<SubSetCount;i++)
	{
		MeshSubSet * pMesh = &SubSetsDA[i];

		if(MtlDA.GetSize())
			HAL->PushMaterial( &MtlDA[pMesh->MtlId] );

		HAL->SetVertexDeclaration(pMesh->Decl);

		HAL->SetStreamSource(0,pMesh->VB,pMesh->VertexStart,pMesh->Stride);
		HAL->SetIndices(pMesh->IB,pMesh->IndexType);

		HAL->SetPrimitiveTopology(pMesh->PrimType);
		HAL->DrawIndexedInstanced(pMesh->IndexCount, 1, pMesh->IndexStart, pMesh->VertexStart);
	}
}

void Mesh::DrawGBuffer()
{
	auto* HAL = gData.Rdr->GetHAL();

	HAL->BindGraphicPipelineState(ShaderMap::BaseMeshVS, ShaderMap::GBufferPassPS);

	U32 SubSetCount = SubSetsDA.GetSize();
	for(int i=0;i<SubSetCount;i++)
	{
		MeshSubSet * pMesh = &SubSetsDA[i];

		HAL->PushMaterial( &MtlDA[pMesh->MtlId] );
		
		HAL->SetVertexDeclaration(pMesh->Decl);
		
		HAL->SetStreamSource(0,pMesh->VB,pMesh->VertexStart,pMesh->Stride);
		HAL->SetIndices(pMesh->IB,pMesh->IndexType);
		//HAL->PushushDrawIndexed(pMesh->PrimType,0,0,pMesh->VertexCount,pMesh->IndexStart,pMesh->IndexCount/3);
	}
}
