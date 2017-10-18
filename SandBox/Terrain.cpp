#include <Terrain.h>
#include <Bitmap.h>

struct VertexTerrain
{
	float	x,y,z;
	float	nx,ny,nz;
	float	u,v;
};
#define VERTEX_TERRAIN_SIZE	sizeof(VertexTerrain)


Terrain::Terrain()
{
	GridSizeX = 4000;
	GridSizeY = 4000;
	m_VB = NULL;
	m_IB = NULL;
}

Terrain::~Terrain()
{
}

#define LODX	50
#define LODY	50

struct Face
{
	U16	Vtx[3];
};

void Terrain::Update(F32 dTime)
{
	int lodx = LODX;
	int lody = LODY;
	if(!m_VB)
	{
		Bitmap * bm = new Bitmap();
		bm->LoadDDS("assets\\textures\\test_dxt1.dds");
		gData.Rdr->CreateTexture(bm);
		m_Mat.SetBitmap(bm);

		VertexElement DeclDesc[] = 
		{
			{ 0, 0, DECL_FMT_FLOAT3, DECL_POSITION },
			{ 0, 12, DECL_FMT_FLOAT3, DECL_NORMAL },
			{ 0, 24, DECL_FMT_FLOAT2, DECL_TEXCOORD0 },
			DECL_END()
		};

		m_VertexDecl = gData.Rdr->CreateVertexDecl( DeclDesc, SHADER_VS_BASE_MESH );

		int num_vertex = (lodx+1)*(lody+1);
		int num_index = (lodx)*(lody)*6;

		DynArray<VertexTerrain,16> VtxDA;

		// Add vertex
		float inc_x = 20.f/(lodx);
		float inc_y = 20.f/(lodx);
		float start_x = -10.f;
		float start_y = -10.f;
		float pos_x,pos_y;
		for(int y=0;y<=lody;y++)
		{
			pos_x = start_x;
			for(int x=0;x<=lodx;x++)
			{
				VertexTerrain * pVtxData = VtxDA.Add();
				pVtxData[0].x = pos_x;
				pVtxData[0].y = 0.5f*cos( 2*sqrt(pos_x*pos_x+start_y*start_y) );
				pVtxData[0].z = start_y;

				pVtxData[0].nx = 0;
				pVtxData[0].ny = 1;
				pVtxData[0].nz = 0;

				pVtxData[0].u = (float)x/(float)lodx;
				pVtxData[0].v = (float)y/(float)lody;

				pos_x += inc_x;
			}
			start_y += inc_y;
		}

		// Build faces
		DynArray<Face,16> FaceDA;
		for(int j=0;j<lody;j++)
		{
			for(int i=0;i<lodx;i++)
			{
				Face* f = FaceDA.Add();
				f->Vtx[0] = (j+0)*(lodx+1)+(i+0);
				f->Vtx[1] = (j+0)*(lodx+1)+(i+1);
				f->Vtx[2] = (j+1)*(lodx+1)+(i+0);
				f = FaceDA.Add();
				f->Vtx[0] = (j+0)*(lodx+1)+(i+1);
				f->Vtx[1] = (j+1)*(lodx+1)+(i+1);
				f->Vtx[2] = (j+1)*(lodx+1)+(i+0);
			}
		}

		// Generate normals
		{
			// Face normals
			DynArray<Vec4f,16> FaceNormalDA;
			FaceNormalDA.SetSize(FaceDA.GetSize());
			for(int i=0;i<FaceDA.GetSize();i++)
			{
				Vec4f p0( VtxDA[FaceDA[i].Vtx[0]].x, VtxDA[FaceDA[i].Vtx[0]].y, VtxDA[FaceDA[i].Vtx[0]].z, 0.f );
				Vec4f p1( VtxDA[FaceDA[i].Vtx[1]].x, VtxDA[FaceDA[i].Vtx[1]].y, VtxDA[FaceDA[i].Vtx[1]].z, 0.f );
				Vec4f p2( VtxDA[FaceDA[i].Vtx[2]].x, VtxDA[FaceDA[i].Vtx[2]].y, VtxDA[FaceDA[i].Vtx[2]].z, 0.f );
				Vec4f v0 = p1 - p0;
				Vec4f v1 = p2 - p0;
				Vec4f n;
				Vec4::Cross(&n,&v1,&v0);
				if(n.y<0)
					int cul = 0;
				Vec4::Normalize(&n,&n);
				FaceNormalDA[i]=n;
			}
			// Build face adjacency for each vertex and compute smoothed normal
			for(int i=0;i<VtxDA.GetSize();i++)
			{
				VertexTerrain* vtx = &VtxDA[i];
				DynArray<int,16> faceAdjacency;
				for(int j=0;j<FaceDA.GetSize();j++)
				{
					if( FaceDA[j].Vtx[0]==i ||
						FaceDA[j].Vtx[1]==i ||
						FaceDA[j].Vtx[2]==i )
						faceAdjacency.Add(j);
				}
				// Build normal
				Vec4f n(0.f,0.f,0.f,0.f);
				for(int j=0;j<faceAdjacency.GetSize();j++)
				{
					Vec4f fn = FaceNormalDA[faceAdjacency[j]];
					n = n + fn;
				}
				Vec4::Normalize(&n,&n);
				vtx->nx = n.x;
				vtx->ny = n.y;
				vtx->nz = n.z;
			}
		}

MESSAGE("CREATE VBIB for TERRAIN");

		// Generate runtime buffers
		m_VB = gData.Rdr->CreateVertexBuffer(num_vertex*VERTEX_TERRAIN_SIZE,0,NULL);
		m_IB = gData.Rdr->CreateIndexBuffer(num_index*sizeof(U16),0,FMT_IDX_16,NULL);

		VertexTerrain * pVtxData;
		if(m_VB && m_VB->Lock(0,0,(void**)&pVtxData,0)==true)
		{
			memcpy(pVtxData,VtxDA.GetArrayPtr(),VtxDA.GetArraySize());
			m_VB->Unlock();
		}

		U16 * pIdxData;
		if(m_IB && m_IB->Lock(0,0,(void**)&pIdxData,0)==true)
		{
			memcpy(pIdxData,FaceDA.GetArrayPtr(),FaceDA.GetArraySize());
			m_IB->Unlock();
		}

		m_Node.SetIdentity();
	}

	// Rotate 180° per sec
	/*static float angle = 0;
	float angle_inc = dTime*2.f/6.28f;
	angle += angle_inc;
	*reinterpret_cast<XMMATRIX*>(&m_Node) = XMMatrixRotationY(angle);*/
}

void Terrain::Draw()
{
	gData.Rdr->PushShader( SHADER_VS_BASE_MESH );
	gData.Rdr->PushShader( SHADER_PS_PIXEL_LIT );
	gData.Rdr->PushVertexDeclaration(m_VertexDecl);
	gData.Rdr->PushStreamSource(0,m_VB,0,VERTEX_TERRAIN_SIZE);
	gData.Rdr->PushIndices(m_IB);
	//gData.Rdr->PushBuffers(m_VB,m_IB);

	gData.Rdr->PushMaterial( &m_Mat );

	//gData.Rdr->PushWorldMatrix();
	float startx,starty;
	for(int cellx=0,startx=-40.f;cellx<5;cellx++,startx+=20.f)
		for(int celly=0,starty=-40.f;celly<5;celly++,starty+=20.f)
		{
			// Set cell position
			Mat4x4 m;// = m_Node;
			
			m.SetIdentity();
			m.SetTranslation(startx,0.f,starty);

#if defined(_PC)
			*reinterpret_cast<XMMATRIX*>(&m) = XMMatrixMultiply((CXMMATRIX)m_Node,(CXMMATRIX)m);
#endif

			gData.Rdr->PushWorldMatrix(&m);

			// Draw cell
			int lodx = LODX;
			int lody = LODY;
			int num_vertex = (lodx+1)*(lody+1);
			int num_index = (lodx)*(lody)*6;
			gData.Rdr->PushDrawIndexed(PRIM_TRIANGLELIST,0,0,num_vertex,0,num_index/3);
		}
}

void Terrain::DrawGBuffer()
{
	gData.Rdr->PushShader( SHADER_VS_BASE_MESH );
	gData.Rdr->PushShader( SHADER_PS_PIXEL_GBUFFER );
	gData.Rdr->PushVertexDeclaration(m_VertexDecl);
	gData.Rdr->PushStreamSource(0,m_VB,0,VERTEX_TERRAIN_SIZE);
	gData.Rdr->PushIndices(m_IB);
	//gData.Rdr->PushBuffers(m_VB,m_IB);

	gData.Rdr->PushMaterial( &m_Mat );

	//gData.Rdr->PushWorldMatrix();
	float startx,starty;
	for(int cellx=0,startx=-40.f;cellx<5;cellx++,startx+=20.f)
		for(int celly=0,starty=-40.f;celly<5;celly++,starty+=20.f)
		{
			// Set cell position
			Mat4x4 m;// = m_Node;
			
			m.SetIdentity();
			m.SetTranslation(startx,0.f,starty);

#if defined(_PC)
			*reinterpret_cast<XMMATRIX*>(&m) = XMMatrixMultiply((CXMMATRIX)m_Node,(CXMMATRIX)m);
#endif

			gData.Rdr->PushWorldMatrix(&m);

			// Draw cell
			int lodx = LODX;
			int lody = LODY;
			int num_vertex = (lodx+1)*(lody+1);
			int num_index = (lodx)*(lody)*6;
			gData.Rdr->PushDrawIndexed(PRIM_TRIANGLELIST,0,0,num_vertex,0,num_index/3);
		}
}
