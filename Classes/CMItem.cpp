#include "CMItem.h"

/************************************************************************/
/* ��Ʒ���߻���  
/************************************************************************/
bool CMItemBasic::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CC_BREAK_IF(!CCNode::init());

		m_pMario = pMario;

		//ע��Update����
		this->schedule(schedule_selector(CMItemBasic::OnCallPerFrame));

		return true;
	} while (false);
	CCLog("fun CMItemBasic::init Error!");
	return false;
}

bool CMItemBasic::OnCollisionMario()
{
	do 
	{
		
		return true;
	} while (false);
	CCLog("fun CMItemBasic::OnCollisionMario Error!");
	return false;
}

void CMItemBasic::OnCallPerFrame(float fT)
{
	do 
	{
		OnCollisionMario();
		return;
	} while (false);
	CCLog("fun CMItemBasic::OnCallPerFrame Error!");
}

void CMItemBasic::RefreshCoinWorldPosition( CCPoint WorldPos )
{
	m_MarioTileMapLayerPos = WorldPos;
}

/************************************************************************/
/* �����                               
/************************************************************************/
CMItemCoin * CMItemCoin::CreateItemIcon( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CMItemCoin *pItemCoin = new CMItemCoin();
		if (pItemCoin && pItemCoin->init(ptItemPos,szItemSize,pMario,pMsgRecver))
		{
			pItemCoin->autorelease();
			return pItemCoin;
		}
		CC_SAFE_DELETE(pItemCoin);
		return NULL;
	} while (false);
	CCLog("fun CMHero::create() Error!");
	return NULL;
}

bool CMItemCoin::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CC_BREAK_IF(!CMItemBasic::init(ptItemPos,szItemSize,pMario,pMsgRecver));

		CCSprite* pCoin = CCSprite::create("coin.png");
		pCoin->setAnchorPoint(ccp(0,0));
		CC_BREAK_IF(pCoin==NULL);
		addChild(pCoin,enZOrderFront,enTagCoin);

		m_pReceiver = pMsgRecver;
		m_MarioTileMapLayerPos = ptItemPos;
		
		return true;
	} while (false);
	CCLog("fun CMItemCoin::init Error!");
	return false;
}

bool CMItemCoin::OnCollisionMario()
{
	do 
	{
		CCSprite* pCoin = dynamic_cast<CCSprite*>(getChildByTag(enTagCoin));
		CC_BREAK_IF(pCoin==NULL);
		
		//�ж���������ҵ���ײ��δ�ɹ���
		if (m_pMario->BoundingBox().intersectsRect(CCRectMake(m_MarioTileMapLayerPos.x,m_MarioTileMapLayerPos.y,pCoin->boundingBox().size.width,pCoin->boundingBox().size.height)))
		{
			CCLog("%f	%f",pCoin->getPositionX(),pCoin->getPositionY());
			SendMsg(enMsgCoinCollision,&pCoin,sizeof(pCoin));
		}

		return true;
	} while (false);
	CCLog("fun CMItemCoin::OnCollisionMario Error!");
	return false;
}

void CMItemCoin::OnCallPerFrame( float fT )
{
	do 
	{
		OnCollisionMario();
		return;
	} while (false);
	CCLog("fun CMItemCoin::OnCallPerFrame Error!");
}

/************************************************************************/
/* ����Ģ��                                               
/************************************************************************/
CMItemMashrooms * CMItemMashrooms::CreateItemCMItemMashrooms( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{
		CMItemMashrooms *pItemMashrooms = new CMItemMashrooms();
		if (pItemMashrooms && pItemMashrooms->init(ptItemPos,szItemSize,pMario,pMsgRecver))
		{
			pItemMashrooms->autorelease();
			return pItemMashrooms;
		}
		CC_SAFE_DELETE(pItemMashrooms);
		return NULL;
	} while (false);
	CCLog("fun CMHero::create() Error!");
	return NULL;
}

bool CMItemMashrooms::init( CCPoint ptItemPos,CCSize szItemSize,CMMario *pMario,CMReceiver *pMsgRecver )
{
	do 
	{

		return true;
	} while (false);
	CCLog("fun CMItemMashrooms::init Error!");
	return false;
}

bool CMItemMashrooms::OnCollisionMario()
{
	do 
	{

		return true;
	} while (false);
	CCLog("fun CMItemMashrooms::OnCollisionMario Error!");
	return false;
}