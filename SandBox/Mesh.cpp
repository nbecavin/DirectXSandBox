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
	gData.Rdr->PushWorldMatrix(&m_Node);
	gData.Rdr->UpdateGlobalConstant();

	gData.Rdr->PushShader( SHADER_VS_BASE_MESH );
	gData.Rdr->PushShader( SHADER_PS_PIXEL_LIT );
	gData.Rdr->SetGlobalConstant(SHADER_TYPE_VERTEX);

	U32 SubSetCount = SubSetsDA.GetSize();
	for(int i=0;i<SubSetCount;i++)
	{
		MeshSubSet * pMesh = &SubSetsDA[i];

		if(MtlDA.GetSize())
			gData.Rdr->PushMaterial( &MtlDA[pMesh->MtlId] );

		gData.Rdr->PushVertexDeclaration(pMesh->Decl);

		gData.Rdr->PushStreamSource(0,pMesh->VB,pMesh->VertexStart,pMesh->Stride);
		gData.Rdr->PushIndices(pMesh->IB,pMesh->IndexType);

		gData.Rdr->SetPrimitiveTopology(pMesh->PrimType);
		gData.Rdr->DrawIndexed(pMesh->IndexCount, pMesh->IndexStart, pMesh->VertexStart);
	}
}

void Mesh::DrawGBuffer()
{
	gData.Rdr->PushWorldMatrix(&m_Node);

	gData.Rdr->PushShader( SHADER_VS_BASE_MESH );
	gData.Rdr->PushShader( SHADER_PS_PIXEL_GBUFFER );

	U32 SubSetCount = SubSetsDA.GetSize();
	for(int i=0;i<SubSetCount;i++)
	{
		MeshSubSet * pMesh = &SubSetsDA[i];

		gData.Rdr->PushMaterial( &MtlDA[pMesh->MtlId] );

		gData.Rdr->PushVertexDeclaration(pMesh->Decl);

		gData.Rdr->PushStreamSource(0,pMesh->VB,pMesh->VertexStart,pMesh->Stride);
		gData.Rdr->PushIndices(pMesh->IB,pMesh->IndexType);
		gData.Rdr->PushDrawIndexed(pMesh->PrimType,0,0,pMesh->VertexCount,pMesh->IndexStart,pMesh->IndexCount/3);
	}
}
