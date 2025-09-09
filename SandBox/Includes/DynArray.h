#ifndef __DYNARRAY_HH__
#define __DYNARRAY_HH__

template<class T,int granularity> class DynArray
{
public:
	T * Datas;
	U32 Size,Reserve;

	DynArray() 
	{
		Size = 0;
		Reserve = 0;
		Datas = NULL;
	}

	inline T*  GetArrayPtr() const { return Datas; }
	inline U32 GetArraySize() const { return sizeof(T)*Size; }
	inline U32 GetSize() const { return Size; }

	T& operator[](U32 Id)
	{
		return Datas[Id];
	}

	T* Add()
	{
		if(!Reserve)
		{
			Reserve=granularity;
			Realloc(Size+Reserve);
		}

		new(&Datas[Size]) T;
		Reserve--;
		Size++;
		return &Datas[Size-1];
	}

	U32 Add(T& _item)
	{
		if(!Reserve)
		{
			Reserve=granularity;
			Realloc(Size+Reserve);
		}

		new(&Datas[Size]) T(_item);
		Reserve--;
		Size++;
		return Size-1;
	}

	void Realloc(U32 NewSize)
	{
		if(Datas==NULL)
			Datas = (T*) malloc( NewSize*sizeof(T) );
		else
			Datas = (T*) realloc( Datas, NewSize*sizeof(T) );
	}

	void SetSize(U32 NewSize)
	{
		if(NewSize>Size)
		{
			// Realloc
			Realloc(NewSize);
			Size = NewSize;
			Reserve = 0;
		}
		else
		{
			Reserve = Size - NewSize;
			Size = NewSize;
		}
	}

};

#endif //__DYNARRAY_HH__
