#ifndef __ASSET_CACHE_HH__
#define __ASSET_CACHE_HH__

class GraphObject;

namespace asset
{
	enum Type {
		BITMAP,
		MESH
	};

	class Cache {
	public:
		
		bool LoadAsset(Type _type,char * path,GraphObject ** oAsset);
		bool Exist(char * path);

		static Cache& GetInstance() {
			static Cache theCache;
			return theCache;
		}

	private:
		Cache() {}

		struct Entry {
			char			name[1024];
			Type			type;
			GraphObject *	ptr;
		}; // This is a cache entry

		DynArray<Entry,1024> m_AssetDA;
	};
}

#endif //__ASSET_CACHE_HH__
