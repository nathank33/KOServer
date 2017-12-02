#include "stdafx.h"
#include "Map.h"
#include "KnightsManager.h"
#include "KingSystem.h"
#include "DBAgent.h"
using namespace std;
using std::string;

void CUser::ItemRepurchase(Packet & pkt)
{ 
	Packet result(WIZ_ITEM_TRADE);
	uint8 subcode = 0;

	pkt >> subcode;
		if (subcode == 4)
		SendItemRepurchase();
		else 
			if (subcode == 2)
			{
				uint8 siranumarasi = -1;
				uint32	ItemID;
				pkt >> siranumarasi >> ItemID;

				_REPURCHASE_TYPE * nRep = RepurchaseList.GetData(siranumarasi);
				_ITEM_TABLE * nTable = g_pMain->GetItemPtr(nRep->ItemID);


					if(nRep == nullptr 
						|| nTable == nullptr)
					{
						result << uint8(2) << uint8(2) << uint16(0); // Re-purchase period has been expired.
						Send(&result);
						return;
					}


					if ((uint32(UNIXTIME) - nRep->SellTime) > 3 * DAY)
					{
						result << uint8(2) << uint8(2) << uint16(-6); // -6 Re-purchase period has been expired.
						Send(&result);
						return;
					}
							
					uint32 real_prices;
					if (nTable->m_iBuyPrice > 1050000000)
						real_prices = nTable->m_iBuyPrice;
					else
						real_prices = nTable->m_iBuyPrice * 2;
		
					if (real_prices > m_iGold)
					{
						result << uint8(2) << uint8(2) << uint16(-8); // You don't have enough Coins. 
						Send(&result);
						return;
					}

					int8 pos;
					bool bNewItem = true;
					pos = FindSlotForItem(nRep->ItemID, 1);
					if (pos < 0)
					{
						result << uint8(2) << uint8(2) << uint16(-4); // Not enough inventory slot 
						Send(&result);
						return;
					}


				_ITEM_DATA *pItem = GetItem(pos);
					if (pItem->nNum != 0 || pItem == nullptr) 
						bNewItem = false;

						
				pItem->nNum = nRep->ItemID;
				pItem->sCount = 1;
				pItem->nSerialNum = g_pMain->GenerateItemSerial();
				pItem->sDuration = nTable->m_sDuration;
				g_DBAgent.UpdateRePurchaseType(1,GetName(),nRep);
				SendStackChange(nRep->ItemID, m_sItemArray[pos].sCount, m_sItemArray[pos].sDuration, pos - SLOT_MAX, true);
				RepurchaseList.DeleteData(nRep->number);
				memset(nRep, 0,sizeof(_REPURCHASE_TYPE));
				GoldLose(real_prices);
				g_DBAgent.UpdateUser(GetName(), UPDATE_LOGOUT, this);
				g_DBAgent.UpdateWarehouseData(GetAccountName(), UPDATE_LOGOUT, this);
				SendItemRepurchase();
			}


}

void CUser::SendItemRepurchase()
{
	Packet result(WIZ_ITEM_TRADE,uint8(5));
	result << uint8(1);
result << uint8(1);
result << uint8(RepurchaseList.GetSize());

foreach_stlmap(itr,RepurchaseList)
{
	_ITEM_TABLE * pTable = g_pMain->GetItemPtr(itr->second->ItemID);
	if (pTable == nullptr)
		continue;

	uint32 BuyPrice = pTable->m_iBuyPrice; 
	uint32 SellPrice = pTable->m_iSellPrice;

	if(SellPrice == SellTypeFullPrice)
		continue;

	if (pTable->m_iBuyPrice > 1050000000)
			BuyPrice = pTable->m_iBuyPrice;
		else
			BuyPrice = pTable->m_iBuyPrice * 2;

	result << uint8(itr->second->number)
	    << itr->second->ItemID
		<< uint32(BuyPrice)
		<< uint32(uint32(UNIXTIME) - uint32(itr->second->SellTime));
}

	Send(&result); 
}
