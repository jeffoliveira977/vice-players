//-----------------------------------------------------
//
// VC:Players Multiplayer Modification For GTA:VC
// Copyright 2010 GTA:Online team
//
// File Author(s): adamix
// License: See LICENSE in root directory
//
//-----------------------------------------------------
#include "netgame.h"
extern CNetGame *pNetGame;

CObject::CObject(int iModel, Vector3 * vecPos, Vector3 * vecRot)
{
	memcpy(&m_vecPos, vecPos, sizeof(Vector3));
	memcpy(&m_vecRot, vecRot, sizeof(Vector3));

	m_iModel = iModel;

	SpawnForWorld();
}

CObject::~CObject()
{

}

void CObject::SpawnForPlayer(EntityId playerId)
{
	RakNet::BitStream bsSend;

	bsSend.Write(m_ObjectID);
	bsSend.Write(m_iModel);
	bsSend.Write(m_vecPos);
	bsSend.Write(m_vecRot);
	
	pNetGame->GetRPC4()->Call("ObjectSpawn", &bsSend, HIGH_PRIORITY, RELIABLE, 0, pNetGame->GetRakPeer()->GetSystemAddressFromIndex(playerId), 0);
}

void CObject::SpawnForWorld()
{
	CPlayerPool * pPlayerPool = pNetGame->GetPlayerPool();
	for(EntityId i = 0; i < MAX_PLAYERS; i++) {
		if(pPlayerPool->GetSlotState(i)) {
			SpawnForPlayer(i);
		}
	}
}
