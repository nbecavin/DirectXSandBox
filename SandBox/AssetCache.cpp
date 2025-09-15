#include <AssetCache.h>

#include <Bitmap.h>

namespace asset
{
	bool Cache::LoadAsset(Type _type,const char * path,GraphObject ** oAsset)
	{
		for(U32 i=0;i<m_AssetDA.GetSize();i++)
		{
			Entry & anEntry = m_AssetDA[i];
			if(anEntry.type==_type && !strcmp(path,anEntry.name))
			{
				*oAsset = anEntry.ptr;
				return false;
			}
		}

		// Create a new asset resource
		switch(_type){
		case BITMAP:
			*oAsset=new Bitmap();
			break;
		case MESH:
			break;
		default:
			*oAsset=NULL;
			break;
		}

		// Add cache entry
		Entry * newEntry = m_AssetDA.Add();
		strcpy(newEntry->name,path);
		newEntry->ptr = *oAsset;
		newEntry->type = _type;

		return true;
	}

	void Cache::DeleteAsset(GraphObject* oAsset)
	{
		for (U32 i = 0; i < m_AssetDA.GetSize(); i++)
		{
			Entry& anEntry = m_AssetDA[i];
			if (anEntry.ptr == oAsset)
			{
				delete oAsset;
				return;
			}
		}
	}


	bool Cache::Exist(char * path)
	{
		FILE * fp = fopen(path,"r");
		if(fp)
		{
			fclose(fp);
			return true;
		}
		else
			return false;
	}

};
