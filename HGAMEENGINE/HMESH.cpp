#include "HMESH.h"


void HMESH::VB(const Game_String& _Name) 
{
	m_VB = HVBBUFFER::Find(_Name);

	if (nullptr == m_VB)
	{
		AMSG(L"���ؽ� ���۰� nullptr �Դϴ�.");
	}

}
void HMESH::IB(const Game_String& _Name) 
{
	m_IB = HIBBUFFER::Find(_Name);

	if (nullptr == m_VB)
	{
		AMSG(L"�ε��� ���۰� nullptr �Դϴ�.");
	}
}

void HMESH::Render() {

	//Game_Ptr<HVBBUFFER> VB = HVBBUFFER::Find(L"2DCOLORRECT");
	//Game_Ptr<HIBBUFFER> IB = HIBBUFFER::Find(L"2DCOLORRECT");
	m_VB->Setting();
	m_IB->Setting();

	Game_Device::MAINOBJ()->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Game_Device::MAINOBJ()->Context()->DrawIndexed(m_IB->Count(), 0, 0);
}

void HMESH::VB(Game_Ptr<HVBBUFFER> _VB)
{
	if (nullptr == _VB)
	{
		return;
	}

	m_VB = _VB;
}
void HMESH::IB(Game_Ptr<HIBBUFFER> _IB) 
{
	if (nullptr == _IB)
	{
		return;
	}

	m_IB = _IB;
}