//----------------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2010 VC-Players Team
//
// File Author(s): adamix
// License: See LICENSE in root directory
//
//----------------------------------------------------------


#include "../main.h"

extern CGame * pGame;
extern CChatWindow * pChatWindow;
extern CNetGame * pNetGame;

#define MAX_SPHERES 16
#define VAR_Spheres 0x811528

struct SPHERE_TYPE
{
	BYTE byteInUse;
	BYTE pad[1];
	WORD wHandle;
	DWORD dwUniqueId;
	Vector3 vecCenter;
	float fRadius;
};

SPHERE_TYPE * GetSphereFromIndex(EntityId index)
{
	if(index >= MAX_SPHERES)
	{
		return NULL;
	}

	return (SPHERE_TYPE *)(VAR_Spheres + (index * sizeof(SPHERE_TYPE)));
}

EntityId GetFreeSphereIndex()
{
	for(EntityId i = 0; i < MAX_SPHERES; i++)
	{
		SPHERE_TYPE * pSphere = GetSphereFromIndex(i);

		if(pSphere && pSphere->byteInUse == 0)
		{
			return i;
		}
	}

	return 0xFF;
}

EntityId CreateSphere(Vector3 * vecCenter, float fRadius)
{
	EntityId index = GetFreeSphereIndex();

	if(index != 0xFF)
	{
		SPHERE_TYPE * pSphere = GetSphereFromIndex(index);
		pSphere->byteInUse = 1;
		pSphere->dwUniqueId = index + 0xDEADBEEF;
		memcpy(&pSphere->vecCenter, vecCenter, sizeof(Vector3));
		pSphere->fRadius = fRadius;
	}

	return index;
}

float GetDistanceBetweenPoints3D(float x, float y, float z, float xx, float yy, float zz)
{
        float newx = xx - x;
        float newy = yy - y;
        float newz = zz - z;

        return sqrt(newx * newx + newy * newy + newz * newz);
}


CCheckpoint::CCheckpoint(EntityId id, Vector3 vecPos, BYTE type, float radius)
{
	m_iID = id;
	memcpy(&m_vecPos, &vecPos, sizeof(Vector3));
	m_byteType = type;
	m_fRadius = radius;
	m_dwSphere = NULL;
	m_dwMarker = NULL;
	m_bInCP = false;

	m_dwSphere = CreateSphere(&vecPos, radius);
	ScriptCommand(&create_icon_marker_without_sphere, vecPos.X, vecPos.Y, vecPos.Z, 0, &m_dwMarker);
	ScriptCommand(&set_marker_color, m_dwMarker, 5);
	ScriptCommand(&show_on_radar, m_dwMarker, 2);
}

CCheckpoint::~CCheckpoint()
{
	GetSphereFromIndex(m_dwSphere)->byteInUse = 0;
	if(m_dwMarker != NULL)
	{
		ScriptCommand(&disable_marker, m_dwMarker);
	}
}

void CCheckpoint::Process()
{

}

CCheckpoints::CCheckpoints()
{
	for(EntityId id = 0; id < MAX_CPS; id++)
	{
		m_Checkpoints[id] = NULL;
		m_bSlotState[id] = false;
	}
}

CCheckpoints::~CCheckpoints()
{
	
}

void CCheckpoints::New(EntityId id, Vector3 vecPos, BYTE type, float radius)
{
	if(GetSlotState(id) == true) 
	{
		Delete(id);
	}

	m_Checkpoints[id] = new CCheckpoint(id, vecPos, type, radius);
	if(m_Checkpoints[id])
	{
		m_bSlotState[id] = true;
	}
}

void CCheckpoints::Delete(EntityId id)
{
	if(m_bSlotState[id] == false) return;
	delete m_Checkpoints[id];
	m_Checkpoints[id] = NULL;
	m_bSlotState[id] = false;
}

void CCheckpoints::Process()
{
	for(EntityId id = 0; id < MAX_CPS; id++)
	{
		if(!GetSlotState(id)) continue;
		m_Checkpoints[id]->Process();
	}
}
