#pragma once

namespace SandBoxEditor {

	using namespace System;

	ref class Material
	{
	public:
		Material(void);
		virtual ~Material(void);
	
	private:
		String^			FriendlyName;
		String^			RscName;
	};

};

