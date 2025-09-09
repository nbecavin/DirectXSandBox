#include <Mesh.h>
#include <Bitmap.h>

#if defined(_PC)
#include <MeshSDKMesh.h>

struct DXMesh {
public:
	bool CreateDXSDKMESHFromMemory( U8* pData, U32 DataBytes,bool bCreateAdjacencyIndices, bool bCopyStatic );
 
    //These are the pointers to the two chunks of data loaded in from the mesh file
    BYTE* m_pStaticMeshData;
    BYTE* m_pHeapData;
    BYTE* m_pAnimationData;
    BYTE** m_ppVertices;
    BYTE** m_ppIndices;

    //General mesh info
    SDKMESH_HEADER* m_pMeshHeader;
    SDKMESH_VERTEX_BUFFER_HEADER* m_pVertexBufferArray;
    SDKMESH_INDEX_BUFFER_HEADER* m_pIndexBufferArray;
    SDKMESH_MESH* m_pMeshArray;
    SDKMESH_SUBSET* m_pSubsetArray;
    SDKMESH_FRAME* m_pFrameArray;
    SDKMESH_MATERIAL* m_pMaterialArray;

    // Adjacency information (not part of the m_pStaticMeshData, so it must be created and destroyed separately )
    SDKMESH_INDEX_BUFFER_HEADER* m_pAdjacencyIndexBufferArray;

    //Animation (TODO: Add ability to load/track multiple animation sets)
    SDKANIMATION_FILE_HEADER* m_pAnimationHeader;
    SDKANIMATION_FRAME_DATA* m_pAnimationFrameData;
};

void Mesh::Load(const char * fname)
{
    HRESULT hr = S_OK;

	const char aliaspath[] = "..\\GameDB";

    // Find the path for the file
//    V_RETURN( DXUTFindDXSDKMediaFileCch( m_strPathW, sizeof( m_strPathW ) / sizeof( WCHAR ), szFileName ) );
	char dir[1024];
	char name[1024];

	sprintf(name, "%s\\%s", aliaspath, fname);
	_splitpath(name,NULL,dir,NULL,NULL);

    // Open the file
	HANDLE m_hFile = CreateFileA( name, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( INVALID_HANDLE_VALUE == m_hFile )
	{
		MESSAGE("failed to load mesh : %s", name);
        return ;
	}

	MESSAGE("Loading mesh : %s", name);

    // Change the path to just the directory
    /*WCHAR* pLastBSlash = wcsrchr( m_strPathW, L'\\' );
    if( pLastBSlash )
        *( pLastBSlash + 1 ) = L'\0';
    else
        *m_strPathW = L'\0';

    WideCharToMultiByte( CP_ACP, 0, m_strPathW, -1, m_strPath, MAX_PATH, NULL, FALSE );*/

    // Get the file size
	FILE_STANDARD_INFO fileInfo;
	GetFileInformationByHandleEx( m_hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo) );

    LARGE_INTEGER FileSize;
    //GetFileSizeEx( m_hFile, &FileSize );
	UINT cBytes = fileInfo.EndOfFile.LowPart;//FileSize.LowPart;

    // Allocate memory
    BYTE * m_pStaticMeshData = new BYTE[ cBytes ];
    if( !m_pStaticMeshData )
    {
        CloseHandle( m_hFile );
        return ;
    }

    // Read in the file
    DWORD dwBytesRead;
    if( !ReadFile( m_hFile, m_pStaticMeshData, cBytes, &dwBytesRead, NULL ) )
        hr = E_FAIL;

    CloseHandle( m_hFile );

    if( SUCCEEDED( hr ) ) 
    {
		DXMesh _mesh;
        hr = _mesh.CreateDXSDKMESHFromMemory( m_pStaticMeshData,
                               cBytes,
                               false,//bCreateAdjacencyIndices,
                               false/*,
                               pLoaderCallbacks11,
                               pLoaderCallbacks9*/ );
        if( FAILED( hr ) )
            delete []m_pStaticMeshData;

		// Build mesh
		if( !m_pStaticMeshData || !_mesh.m_pFrameArray )
			return;

		U32 iFrame = 0;

loadmesh:

		if( _mesh.m_pFrameArray[iFrame].Mesh != INVALID_MESH )
		{
			SDKMESH_MESH * pMesh = &_mesh.m_pMeshArray[_mesh.m_pFrameArray[iFrame].Mesh];
			for(int i=0;i<pMesh->NumSubsets;i++)
			{
				MeshSubSet * pItem = SubSetsDA.Add();
				SDKMESH_SUBSET * pSubSet = &_mesh.m_pSubsetArray[pMesh->pSubsets[i]];
				pItem->IndexStart = (U32)pSubSet->IndexStart;
				pItem->IndexCount = (U32)pSubSet->IndexCount;
				pItem->VertexStart = (U32)pSubSet->VertexStart;
				pItem->VertexCount = (U32)pSubSet->VertexCount;
				pItem->PrimType = (pSubSet->PrimitiveType==0) ? PRIM_TRIANGLELIST : PRIM_TRIANGLESTRIP;

				pItem->VB = _mesh.m_pVertexBufferArray[pMesh->VertexBuffers[0]].pVB;
				pItem->IB = _mesh.m_pIndexBufferArray[pMesh->IndexBuffer].pIB;
				pItem->IndexType = (_mesh.m_pIndexBufferArray[pMesh->IndexBuffer].IndexType==IT_32BIT)?FMT_IDX_32:FMT_IDX_16;

				VertexElement DeclDesc[32];
				VertexElement * pElt = DeclDesc;
				D3DVERTEXELEMENT9 * pDecl = _mesh.m_pVertexBufferArray[pMesh->VertexBuffers[0]].Decl;
				while(pDecl->Type!=D3DDECLTYPE_UNUSED)
				{
					pElt->StreamId = pDecl->Stream;
					pElt->Offset = pDecl->Offset;
					switch(pDecl->Type){
					case D3DDECLTYPE_FLOAT4:
						pElt->Type = DECL_FMT_FLOAT4; break;
					case D3DDECLTYPE_FLOAT3:
						pElt->Type = DECL_FMT_FLOAT3; break;
					case D3DDECLTYPE_FLOAT2:
						pElt->Type = DECL_FMT_FLOAT2; break;
					case D3DDECLTYPE_FLOAT1:
						pElt->Type = DECL_FMT_FLOAT1; break;
					case D3DDECLTYPE_D3DCOLOR:
						pElt->Type = DECL_FMT_COLOR; break;
					case D3DDECLTYPE_UBYTE4:
						pElt->Type = DECL_FMT_UBYTE4; break;
					case D3DDECLTYPE_SHORT2:
						pElt->Type = DECL_FMT_SHORT2; break;
					case D3DDECLTYPE_SHORT4:
						pElt->Type = DECL_FMT_SHORT4; break;
					case D3DDECLTYPE_UBYTE4N:
						pElt->Type = DECL_FMT_UBYTE4N; break;
					case D3DDECLTYPE_SHORT2N:
						pElt->Type = DECL_FMT_SHORT2N; break;
					case D3DDECLTYPE_SHORT4N:
						pElt->Type = DECL_FMT_SHORT4N; break;
					case D3DDECLTYPE_USHORT2N:
						pElt->Type = DECL_FMT_USHORT2N; break;
					case D3DDECLTYPE_USHORT4N:
						pElt->Type = DECL_FMT_USHORT4N; break;
					case D3DDECLTYPE_UDEC3:
						pElt->Type = DECL_FMT_UDEC3; break;
					case D3DDECLTYPE_DEC3N:
						pElt->Type = DECL_FMT_DEC3N; break;
					case D3DDECLTYPE_FLOAT16_2:
						pElt->Type = DECL_FMT_FLOAT16_2; break;
					case D3DDECLTYPE_FLOAT16_4:
						pElt->Type = DECL_FMT_FLOAT16_4; break;
					default:
						pElt->Type = -1;
						break;
					}
					switch(pDecl->Usage){
					case D3DDECLUSAGE_POSITION:
						pElt->Semantic = DECL_POSITION+pDecl->UsageIndex; break;
					case D3DDECLUSAGE_TEXCOORD:
						pElt->Semantic = DECL_TEXCOORD0+pDecl->UsageIndex; break;
					case D3DDECLUSAGE_NORMAL:
						pElt->Semantic = DECL_NORMAL; break;
					case D3DDECLUSAGE_TANGENT:
						pElt->Semantic = DECL_TANGENT; break;
					case D3DDECLUSAGE_BINORMAL:
						pElt->Semantic = DECL_BINORMAL; break;
					case D3DDECLUSAGE_COLOR:
						pElt->Semantic = DECL_COLOR0; break;
					default:
						pElt->Semantic = -1;
					}
					pDecl++; pElt++;
				}
				pElt[0].END();
				pItem->Decl = gData.Rdr->CreateVertexDecl( DeclDesc, SHADER_VS_BASE_MESH );
				pItem->Stride = _mesh.m_pVertexBufferArray[pMesh->VertexBuffers[0]].StrideBytes;
				pItem->MtlId = pSubSet->MaterialID;
			}

		}

		// Render our children
		if( _mesh.m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
		{
		//	RenderFrame( m_pFrameArray[iFrame].ChildFrame, bAdjacent, pd3dDeviceContext, iDiffuseSlot, 
		//				 iNormalSlot, iSpecularSlot );
			iFrame = _mesh.m_pFrameArray[iFrame].ChildFrame;
			if( _mesh.m_pFrameArray[iFrame].Mesh != INVALID_MESH )
				goto loadmesh;
		}

		// Render our siblings
		if( _mesh.m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
		{
		//	RenderFrame( m_pFrameArray[iFrame].SiblingFrame, bAdjacent, pd3dDeviceContext, iDiffuseSlot, 
		//				 iNormalSlot, iSpecularSlot );
			int cul = 0;
		}


		// Load materials
		SDKMESH_MATERIAL * pMaterials = _mesh.m_pMaterialArray;
		for( UINT m = 0; m < _mesh.m_pMeshHeader->NumMaterials; m++ )
		{
			Material * pMat = MtlDA.Add();

			char tex[1024];
			Bitmap * bm;

			// load textures
			if( pMaterials[m].DiffuseTexture[0] != 0 )
			{
				sprintf(tex,"%s\\%s",dir,pMaterials[m].DiffuseTexture);

				asset::Cache& asset = asset::Cache::GetInstance();
				Bool bNew = asset.LoadAsset(asset::Type::BITMAP,tex,(GraphObject**)&bm);
				if (bNew)
				{
					bm->LoadDDS(tex);
					if (bm->GetSx() && bm->GetSy())
					{
						gData.Rdr->CreateTexture(bm);
					}
				}
				if (bm)
				{
					pMat->SetBitmap(bm, MTL_STAGE_ALBEDO);
				}

				// sponza super machin truc
				// Try to find normal maps ... and spec maps ...
				{
					
					int	  pathlen = strlen(tex);
					char* origname = tex+pathlen;
					for(int i=pathlen-1;i>=0;i--,origname--)
						if(tex[i]=='\\' || tex[i]=='/')
							break;

					static char stringtoreplace[][16] = {
						"diff.dds",
						".dds",
						"dif.dds"
					};
					static char stringtoadd[][16] = {
						"ddn.dds",
						"_ddn.dds",
						"ddn.dds"
					};

					char newname[1024];
					char pathname[1024];
					for(int i=0;i<3;i++)
					{
						strcpy(newname,origname);
						char * token = strstr(newname,stringtoreplace[i]);
						if(token)
						{
							strcpy(token,stringtoadd[i]);
							sprintf(pathname,"%s\\%s",dir,newname);
							if(asset.Exist(pathname))
								strcpy(pMaterials[m].NormalTexture,newname);
						}
					}
				}
			}
			if( pMaterials[m].NormalTexture[0] != 0 )
			{
				sprintf(tex,"%s\\%s",dir,pMaterials[m].NormalTexture);

				asset::Cache& asset = asset::Cache::GetInstance();
				Bool bNew = asset.LoadAsset(asset::Type::BITMAP,tex,(GraphObject**)&bm);
				if(bNew)
				{
					bm->LoadDDS(tex);
					gData.Rdr->CreateTexture(bm);
				}
				pMat->SetBitmap(bm,MTL_STAGE_NORMAL);
			}
			if( pMaterials[m].SpecularTexture[0] != 0 )
			{
				sprintf(tex,"%s\\%s",dir,pMaterials[m].SpecularTexture);

				asset::Cache& asset = asset::Cache::GetInstance();
				Bool bNew = asset.LoadAsset(asset::Type::BITMAP,tex,(GraphObject**)&bm);
				if(bNew)
				{
					bm->LoadDDS(tex);
					gData.Rdr->CreateTexture(bm);
				}
				pMat->SetBitmap(bm, MTL_STAGE_ROUGHNESS);
			}
		}
    }
}

bool DXMesh::CreateDXSDKMESHFromMemory( U8* pData, U32 DataBytes,bool bCreateAdjacencyIndices, bool bCopyStatic
										/*,
                                        SDKMESH_CALLBACKS11* pLoaderCallbacks11,
                                        SDKMESH_CALLBACKS9* pLoaderCallbacks9*/ )
{
	m_pHeapData = pData;
	m_pStaticMeshData = pData;

    // Pointer fixup
    m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
    m_pVertexBufferArray = ( SDKMESH_VERTEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                              m_pMeshHeader->VertexStreamHeadersOffset );
    m_pIndexBufferArray = ( SDKMESH_INDEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                            m_pMeshHeader->IndexStreamHeadersOffset );
    m_pMeshArray = ( SDKMESH_MESH* )( m_pStaticMeshData + m_pMeshHeader->MeshDataOffset );
    m_pSubsetArray = ( SDKMESH_SUBSET* )( m_pStaticMeshData + m_pMeshHeader->SubsetDataOffset );
    m_pFrameArray = ( SDKMESH_FRAME* )( m_pStaticMeshData + m_pMeshHeader->FrameDataOffset );
    m_pMaterialArray = ( SDKMESH_MATERIAL* )( m_pStaticMeshData + m_pMeshHeader->MaterialDataOffset );

    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }

    // error condition
    if( m_pMeshHeader->Version != SDKMESH_FILE_VERSION )
    {
        //hr = E_NOINTERFACE;
        goto Error;
    }

    // Setup buffer data pointer
    BYTE* pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Get the start of the buffer data
    UINT64 BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Create VBs
    m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        BYTE* pVertices = NULL;
        pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );
		UINT byteSize = ( UINT ) m_pVertexBufferArray[i].SizeBytes;
		m_pVertexBufferArray[i].pVB = gData.Rdr->CreateVertexBuffer( byteSize, 0, pVertices );
        m_ppVertices[i] = pVertices;
    }

    // Create IBs
    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );
		/*if(m_pIndexBufferArray[i].IndexType==IT_32BIT)
		{
			// We need to convert into 16 bits indices
			// convert inplace ---- optimize later
			unsigned short * pU16 = (unsigned short *)pIndices;
			unsigned long * pU32 = (unsigned long *)pIndices;
			for(int _i=0; _i<(m_pIndexBufferArray[i].SizeBytes/4); _i++)
				pU16[_i] = (unsigned short)pU32[_i];
			m_pIndexBufferArray[i].SizeBytes /= 2;
			m_pIndexBufferArray[i].IndexType=IT_16BIT;
		}*/
		m_pIndexBufferArray[i].pIB = gData.Rdr->CreateIndexBuffer( m_pIndexBufferArray[i].SizeBytes, 0, (m_pIndexBufferArray[i].IndexType==IT_32BIT)?FMT_IDX_32:FMT_IDX_16, pIndices );
		//m_pIndexBufferArray[i].pIB = gData.Rdr->CreateIndexBuffer( m_pIndexBufferArray[i].SizeBytes, 0, FMT_IDX_16, pIndices );
        m_ppIndices[i] = pIndices;
    }

	// Load materials
	for( UINT m = 0; m < m_pMeshHeader->NumMaterials; m++ )
    {
		SDKMESH_MATERIAL * pMaterials = m_pMaterialArray;
		//pMaterials[m].pDiffuseTexture10 = NULL;
  //      pMaterials[m].pNormalTexture10 = NULL;
  //      pMaterials[m].pSpecularTexture10 = NULL;
  //      pMaterials[m].pDiffuseRV10 = NULL;
  //      pMaterials[m].pNormalRV10 = NULL;
  //      pMaterials[m].pSpecularRV10 = NULL;

        // load textures
        if( pMaterials[m].DiffuseTexture[0] != 0 )
        {
            /*pLoaderCallbacks->pCreateTextureFromFile( pd3dDevice,
                                                      pMaterials[m].DiffuseTexture, &pMaterials[m].pDiffuseRV10,
                                                      pLoaderCallbacks->pContext );*/
        }
        if( pMaterials[m].NormalTexture[0] != 0 )
        {
            /*pLoaderCallbacks->pCreateTextureFromFile( pd3dDevice,
                                                      pMaterials[m].NormalTexture, &pMaterials[m].pNormalRV10,
                                                      pLoaderCallbacks->pContext );*/
        }
        if( pMaterials[m].SpecularTexture[0] != 0 )
        {
            /*pLoaderCallbacks->pCreateTextureFromFile( pd3dDevice,
                                                      pMaterials[m].SpecularTexture, &pMaterials[m].pSpecularRV10,
                                                      pLoaderCallbacks->pContext );*/
        }
    }

	return TRUE;

Error:
	return FALSE;
}

#else

void Mesh::Load(char * fname)
{
	//.......................................................
}

#endif //_PC
