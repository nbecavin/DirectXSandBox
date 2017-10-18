#include <SandBoxFile.h>

using namespace fs;

FileError File::Open(const char* _filename, FileFlags _flags)
{
	if(m_Hdl)
		Close();
	m_Hdl = NewFileHandler();
	return m_Hdl->Open(_filename,_flags);
}

void File::Close()
{
	if(m_Hdl)
	{
		m_Hdl->Close();
		delete m_Hdl;
		m_Hdl = NULL;
	}
}

U32 File::Read(void* _buffer,U32 _eltsize,U32 _eltcount)
{
	if(m_Hdl)
		return m_Hdl->Read(_buffer,_eltsize,_eltcount);
	else
		return 0;
}

void File::Write()
{
}

void File::SetPos(FileSeek _seek)
{
	if(!m_Hdl)	return;
	m_Hdl->SetPos(_seek);
}

U32 File::GetPos()
{
	if(!m_Hdl)	return 0;
	return m_Hdl->GetPos();
}

U32 File::GetSize()
{
	if(!m_Hdl)	return 0;
	return m_Hdl->GetSize();
}
