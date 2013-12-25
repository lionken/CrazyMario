﻿#include "CMGameMap.h"

#define CONTROL_UI_HEIGHT	96		//控制UI高度
#define TILE_MAP_VERTICAL	13		//地图瓦片竖直块数
#define JUMP_START_SPEED	8		//跳跃起始速度
#define JUMP_SPEED_MINUS	0.3		//跳跃递减速度
#define DROP_SPEED_PLUS		0.098	//掉落加速度
#define MOVE_SPEED			2		//移动速度

CMGameMap* CMGameMap::CreateGameMap(const char* pFileName,CMMario* pMario)
{
	do 
	{
		CMGameMap* pPlayer = new CMGameMap;
		if (pPlayer && pPlayer->initWithTMXFile(pFileName))
		{
			pPlayer->Init(pMario);
			pPlayer->autorelease();
			return pPlayer;
		}
		delete pPlayer;
	} while (false);
	CCLog("Fun CMGameMap::CreateGameMap Error!");
	return NULL;
}

bool CMGameMap::Init(CMMario* pMario)
{
	do 
	{
		//注册Update函数
		this->schedule(schedule_selector(CMGameMap::OnCallPerFrame));

		m_pMario = pMario;
		m_fMapMove = 0;
		m_fDropSpeedPlus = 0;
		m_fJumpSpeed = 0;
		m_fSpeed = MOVE_SPEED;

		m_bIsLeftKeyDown = false;
		m_bIsRightKeyDown = false;
		m_bIsJumpKeyDown = false;
		m_bIsHeroDead = false;

		m_pArrayOfCoin = CCArray::create();
		m_pArrayOfCoin->retain();
		m_pArrayMonsters = CCArray::create();
		m_pArrayMonsters->retain();
		m_pArrayOfDisappearCoin = CCArray::create();
		m_pArrayOfDisappearCoin->retain();

		//初始化金币显示
		CCTMXLayer* pCoinLayer = layerNamed("coin");
		CC_BREAK_IF(pCoinLayer==NULL);
		//获得地图的瓦片数量
		int nMapHorizontalTileNum = pCoinLayer->boundingBox().size.width/getTileSize().width;
		int nMapVerticalTileNum = pCoinLayer->boundingBox().size.height/getTileSize().height;
		//遍历每片瓦片，寻找金币，建立并加入金币集合
		for (int i = 0;i<nMapHorizontalTileNum;i++)
		{
			for (int j = 0;j<nMapVerticalTileNum;j++)
			{
				if (TileMapPosToTileType(ccp(i,j),m_fMapMove)==enTileTypeCoin)
				{
					//将瓦片地图坐标转换为瓦片地图层坐标
					CCPoint CoinTileMapLayerPos = TileMapPosToTileMapLayerPos(ccp(i,j));
					CMItemCoin* pCoin = CMItemCoin::CreateItemIcon(CoinTileMapLayerPos,getTileSize(),pMario,this);
					if (pCoin==NULL)
					{
						CCLog("Coin init Error!");
					}
					pCoin->setPosition(CoinTileMapLayerPos);
					m_pArrayOfCoin->addObject(pCoin);
					addChild(pCoin);
				}
			}
		}
		
		//初始化怪物显示
		CCTMXObjectGroup* pObjectLayer = objectGroupNamed("objects");
		CC_BREAK_IF(pObjectLayer==NULL);
		CCArray *ObjectArray = pObjectLayer->getObjects();
		CCDictionary *pDic = NULL;
		for (unsigned int i = 0; i < ObjectArray->count(); i++)
		{
			pDic = (CCDictionary *)ObjectArray->objectAtIndex(i);
			int PosX = ((CCString*)pDic->objectForKey("x"))->intValue();
			int PosY = ((CCString*)pDic->objectForKey("y"))->intValue();
			PosY -= this->getTileSize().height;
			CCPoint TileXY = ccp(PosX, PosY);

			CCString *strName = (CCString*)pDic->objectForKey("name");
			CCString *strType = (CCString*)pDic->objectForKey("type");

			// 进行怪物的初始化，先根据名字来判断是不是enemy，再细分enemy类型		
			if (strName->m_sString == "enemy")
			{
				if (strType->m_sString == "mushroom")
				{
					CMMonsterMushrooms *pMonster = CMMonsterMushrooms::CreateMonsterMushrooms(TileXY,pMario,this);
					if (pMonster==NULL)
					{
						CCLog("pMonster==NULL!");
					}
					pMonster->setPosition(ccp(TileXY.x,TileXY.y+pMonster->getContentSize().height/2));
					m_pArrayMonsters->addObject(pMonster);
					addChild(pMonster,enZOrderFront);
				}
			}

		}

		return true;
	} while (false);
	CCLog("Fun CMGameMap::Init Error!");
	return false;
}

CCSprite* CMGameMap::TileMapLayerPosToTileSprite( CCPoint HeroPos,float fMapMove)
{
	do 
	{
		//将层坐标转换为地图瓦片坐标
		int nHeroTilePosX = (HeroPos.x + fMapMove)/this->getTileSize().width;
		int nHeroTempPosY = (HeroPos.y - CONTROL_UI_HEIGHT)/this->getTileSize().height;
		int nHeroTilePosY = TILE_MAP_VERTICAL - nHeroTempPosY;

		//获得地图的各个层
		CCTMXLayer* pCloudLayer = layerNamed("cloud");
		CC_BREAK_IF(pCloudLayer==NULL);
		CCTMXLayer* pBlockLayer = layerNamed("block");
		CC_BREAK_IF(pBlockLayer==NULL);
		CCTMXLayer* pPipeLayer = layerNamed("pipe");
		CC_BREAK_IF(pPipeLayer==NULL);
		CCTMXLayer* pLandLayer = layerNamed("land");
		CC_BREAK_IF(pLandLayer==NULL);
		CCTMXLayer* pTrapLayer = layerNamed("trap");
		CC_BREAK_IF(pTrapLayer==NULL);
		CCTMXObjectGroup* pObjectLayer = objectGroupNamed("objects");
		CC_BREAK_IF(pObjectLayer==NULL);
		CCTMXLayer* pCoinLayer = layerNamed("coin");
		CC_BREAK_IF(pCoinLayer==NULL);
		pCoinLayer->setVisible(false);
		CCTMXLayer* pFlagpoleLayer = layerNamed("flagpole");
		CC_BREAK_IF(pFlagpoleLayer==NULL);

		//若马里奥超过上边界
		if(nHeroTilePosY<0)return NULL;

		CCSprite* pLandSprite = pLandLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pLandSprite!=NULL)
		{
			return pLandSprite;
		}
		CCSprite* pBlockSprite = pBlockLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pBlockSprite!=NULL)
		{
			return pBlockSprite;
		}
		CCSprite* pPipeSprite = pPipeLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));	
		if (pPipeSprite!=NULL)
		{
			return pPipeSprite;
		}
		return NULL;
	} while (false);
	CCLog("fun CMGameMap::HeroPosToTileType Error!");
	return NULL;
}

void CMGameMap::OnCallPerFrame(float dt)
{
	do 
	{
		CCPoint CurMarioPos = m_pMario->getPosition();

		//遍历金币数组，分别将金币的世界坐标更新，用以和Mario碰撞
		CCObject* pObj = NULL;
		CCARRAY_FOREACH(m_pArrayOfCoin,pObj)
		{
			CMItemCoin* pCoin = dynamic_cast<CMItemCoin*>(pObj);
			pCoin->RefreshCoinWorldPosition(ccp(pCoin->getPositionX() - m_fMapMove,pCoin->getPositionY() + CONTROL_UI_HEIGHT));
		}

		//删除需要被删除的金币
		m_pArrayOfCoin->removeObjectsInArray(m_pArrayOfDisappearCoin);
		pObj = NULL;
		CCARRAY_FOREACH(m_pArrayOfDisappearCoin,pObj)
		{
			CMItemCoin* pCoin = dynamic_cast<CMItemCoin*>(pObj);
			if (pCoin==NULL)
			{
				CCLog("pCoin==NULL!");
			}
			pCoin->removeFromParent();
		}
		m_pArrayOfDisappearCoin->removeAllObjects();

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)    

// 		CCMenu* pMenu = dynamic_cast<CCMenu*>(getChildByTag(enTagMenu));
// 		CC_BREAK_IF(pMenu==NULL);
// 		CCMenuItemImage* pLeftKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
// 		CC_BREAK_IF(pLeftKey==NULL);
// 		CCMenuItemImage* pRightKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
// 		CC_BREAK_IF(pRightKey==NULL);
// 		CCMenuItemImage* pJumpKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
// 		CC_BREAK_IF(pJumpKey==NULL);
// 		CCMenuItemImage* pFireKey = dynamic_cast<CCMenuItemImage*>(pMenu->getChildByTag(enTagLeftKey));
// 		CC_BREAK_IF(pFireKey==NULL);

		if(KEY_DOWN(KEY_KEY_K))
		{

		}
		if (KEY_DOWN(KEY_KEY_J))
		{

		}

		if(KEY_DOWN(KEY_KEY_A))
		{
			m_bIsLeftKeyDown = true;
		}
		if(KEY_UP(KEY_KEY_A))
		{
			m_bIsLeftKeyDown = false;
		}
		if(KEY_DOWN(KEY_KEY_D))
		{
			m_bIsRightKeyDown = true;
		}
		if(KEY_UP(KEY_KEY_D))
		{
			m_bIsRightKeyDown = false;
		}
		if(KEY_DOWN(KEY_KEY_W))
		{
			m_bIsJumpKeyDown = true;
		}
		if(KEY_UP(KEY_KEY_W))
		{
			m_bIsJumpKeyDown = false;
		}
		if(KEY_DOWN(KEY_KEY_S))
		{

		}
#endif

		//判断马里奥是否落坑死亡
		if((m_pMario->getPositionY() - CONTROL_UI_HEIGHT)/getTileSize().height<0)
		{
			CCDirector* pDirector = CCDirector::sharedDirector();
			CCScene *pScene = CMGameScene::CreateGameScene();
			pDirector->replaceScene(pScene);
			return;
		}

		CCSprite* pTileSprite1 = NULL;
		CCSprite* pTileSprite2 = NULL;
		CCSprite* pTileSprite3 = NULL;
		//根据变量控制英雄移动及动作
		if (m_bIsLeftKeyDown)
		{
			//用英雄左方的三个瓦片来判断后退碰撞
			pTileSprite1 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX(),m_pMario->getPositionY()+m_pMario->boundingBox().size.height),m_fMapMove);
			pTileSprite2 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX(),m_pMario->getPositionY()+m_pMario->boundingBox().size.height/2),m_fMapMove);
			pTileSprite3 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX(),m_pMario->getPositionY()),m_fMapMove);
			if (pTileSprite1!=NULL || pTileSprite2!=NULL)
			{
				m_pMario->setPosition(CurMarioPos);
			}
			else
			{
				if (m_pMario->boundingBox().getMinX()>0)
				{
					m_pMario->setPositionX(m_pMario->getPositionX()-m_fSpeed);
				}
			}
		}

		pTileSprite1 = NULL;
		pTileSprite2 = NULL;
		pTileSprite3 = NULL;
		if (m_bIsRightKeyDown)
		{
			//用英雄右方的三个瓦片来判断前进碰撞
			pTileSprite1 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width,m_pMario->getPositionY()+m_pMario->boundingBox().size.height),m_fMapMove);
			pTileSprite2 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width,m_pMario->getPositionY()+m_pMario->boundingBox().size.height/2),m_fMapMove);
			pTileSprite3 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width,m_pMario->getPositionY()),m_fMapMove);
			if (pTileSprite1!=NULL || pTileSprite2!=NULL)
			{
				m_pMario->setPosition(CurMarioPos);
			}
			else
			{
				if (m_pMario->getPositionX()>100)
				{
					setPositionX(getPositionX()-m_fSpeed);
					m_fMapMove += m_fSpeed;
				}
				else 
				{
					m_pMario->setPositionX(m_pMario->getPositionX()+m_fSpeed);
				}
			}
		}

		pTileSprite1 = NULL;
		pTileSprite2 = NULL;
		pTileSprite3 = NULL;
		//用英雄下方的三个瓦片来判断掉落碰撞
		pTileSprite1 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width/2,m_pMario->getPositionY()),m_fMapMove);
		pTileSprite2 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+5,m_pMario->getPositionY()),m_fMapMove);
		pTileSprite3 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width-5,m_pMario->getPositionY()),m_fMapMove);
		if (pTileSprite1!=NULL || pTileSprite2!=NULL || pTileSprite3!=NULL)
		{
			m_pMario->setPosition(CurMarioPos);
			m_pMario->setPositionY(m_pMario->getPositionY()+1/*pMario->boundingBox().size.height*0.1*/);
			//掉落速度归零
			m_fDropSpeedPlus = 0;
			//跳跃起始速度
			m_fJumpSpeed = JUMP_START_SPEED;
		}
		else
		{
			m_pMario->setPositionY(m_pMario->getPositionY()-m_fDropSpeedPlus);
			//掉落加速度
			m_fDropSpeedPlus += DROP_SPEED_PLUS;
		}

		pTileSprite1 = NULL;
		pTileSprite2 = NULL;
		pTileSprite3 = NULL;
		//用英雄上方的三个瓦片来判断头顶碰撞
		pTileSprite1 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width/2,m_pMario->getPositionY()+m_pMario->boundingBox().size.height),m_fMapMove);
		pTileSprite2 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+5,m_pMario->getPositionY()+m_pMario->boundingBox().size.height),m_fMapMove);
		pTileSprite3 = TileMapLayerPosToTileSprite(ccp(m_pMario->getPositionX()+m_pMario->boundingBox().size.width-5,m_pMario->getPositionY()+m_pMario->boundingBox().size.height),m_fMapMove);
		if (pTileSprite1!=NULL || pTileSprite2!=NULL || pTileSprite3!=NULL)
		{
			m_pMario->setPosition(CurMarioPos);
			m_pMario->setPositionY(m_pMario->getPositionY()-1);
			//跳跃速度归零
			m_fJumpSpeed = 0;
		}
		else
		{
			//跳跃
			if (m_bIsJumpKeyDown)
			{
				m_pMario->setPositionY(m_pMario->getPositionY()+m_fJumpSpeed);
				//跳跃递减速度
				m_fJumpSpeed -= JUMP_SPEED_MINUS;
			}
		}

		return;
	} while (false);
	CCLog("fun CMGameMap::OnCallPerFrame Error!");
}

enTileType CMGameMap::TileMapPosToTileType( CCPoint HeroPos,float fMapMove )
{
	do 
	{
		//地图瓦片坐标
		int nHeroTilePosX = HeroPos.x;
		int nHeroTilePosY = HeroPos.y;

		//获得地图的各个层
		CCTMXLayer* pCloudLayer = layerNamed("cloud");
		CC_BREAK_IF(pCloudLayer==NULL);
		CCTMXLayer* pBlockLayer = layerNamed("block");
		CC_BREAK_IF(pBlockLayer==NULL);
		CCTMXLayer* pPipeLayer = layerNamed("pipe");
		CC_BREAK_IF(pPipeLayer==NULL);
		CCTMXLayer* pLandLayer = layerNamed("land");
		CC_BREAK_IF(pLandLayer==NULL);
		CCTMXLayer* pTrapLayer = layerNamed("trap");
		CC_BREAK_IF(pTrapLayer==NULL);
		CCTMXLayer* pCoinLayer = layerNamed("coin");
		CC_BREAK_IF(pCoinLayer==NULL);
		CCTMXLayer* pFlagpoleLayer = layerNamed("flagpole");
		CC_BREAK_IF(pFlagpoleLayer==NULL);

		CCSprite* pLandSprite = pLandLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pLandSprite!=NULL)
		{
			return enTileTypeLand;
		}
		CCSprite* pBlockSprite = pBlockLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pBlockSprite!=NULL)
		{
			return enTileTypeBlock;
		}
		CCSprite* pPipeSprite = pPipeLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));	
		if (pPipeSprite!=NULL)
		{
			return enTileTypePipe;
		}
		CCSprite* pCoinSprite = pCoinLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));	
		if (pCoinSprite!=NULL)
		{
			return enTileTypeCoin;
		}
		return enTileTypeNone;
	} while (false);
	CCLog("fun CMGameMap::PosToTileType Error!");
	return enTileTypeNone;
}

void CMGameMap::onExit()
{
	m_pArrayOfCoin->removeAllObjects();
	CC_SAFE_RELEASE(m_pArrayOfCoin);
	m_pArrayMonsters->removeAllObjects();
	CC_SAFE_RELEASE(m_pArrayMonsters);
	m_pArrayOfDisappearCoin->removeAllObjects();
	CC_SAFE_RELEASE(m_pArrayOfDisappearCoin);
	CCTMXTiledMap::onExit();
}

cocos2d::CCPoint CMGameMap::TileMapPosToTileMapLayerPos( CCPoint TilePos )
{
	float fPositionX = TilePos.x * getTileSize().width;
	float fPositionY = SCREEN_HEIGHT - CONTROL_UI_HEIGHT - (TilePos.y * getTileSize().height + getTileSize().height);
	return ccp(fPositionX,fPositionY);
}

void CMGameMap::OnMsgReceive( int enMsg,void* pData,int nSize )
{
	switch (enMsg)
	{
	case enMsgCoinCollision:
		{
			if (sizeof(MsgForCoinCollision)!=nSize)
			{
				CCAssert(false,"sizeof(MsgForCoinCollision)!=nSize");
			}
			this->CoinDisppear((MsgForCoinCollision*)pData);
		}
		break;

	}
}

cocos2d::CCPoint CMGameMap::TileMapLayerPosToWorldPos( CCPoint TileMapLayerPos,float m_fMapMove )
{
	float fPositionY = TileMapLayerPos.y;
	float fPositionX = TileMapLayerPos.x - m_fMapMove;
	return ccp(fPositionX,fPositionY);
}

void CMGameMap::CoinDisppear(MsgForCoinCollision* pData )
{
	m_pArrayOfDisappearCoin->addObject(pData->pCoin);
}
