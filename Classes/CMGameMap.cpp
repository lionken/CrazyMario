﻿#include "CMGameMap.h"


CMGameMap* CMGameMap::CreateGameMap(const char* pFileName)
{
	do 
	{
		CMGameMap* pPlayer = new CMGameMap;
		if (pPlayer && pPlayer->initWithTMXFile(pFileName))
		{
			pPlayer->Init();
			pPlayer->autorelease();
			return pPlayer;
		}
		delete pPlayer;
	} while (false);
	CCLog("Fun CMGameMap::CreateGameMap Error!");
	return NULL;
}

bool CMGameMap::Init()
{
	do 
	{


		return true;
	} while (false);
	CCLog("Fun CMGameMap::Init Error!");
	return false;
}

TileType CMGameMap::HeroPosToTileType( CCRect HeroRect,float fMapMove )
{
	do 
	{
		//将层坐标转换为地图瓦片坐标
		int nHeroTilePosX = (HeroRect.getMinX() + fMapMove)/this->getTileSize().width;
		int nHeroTempPosY = (HeroRect.getMinY() - 96)/this->getTileSize().height;
		int nHeroTilePosY = 13 - nHeroTempPosY;

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
		CCTMXLayer* pFlagpoleLayer = layerNamed("flagpole");
		CC_BREAK_IF(pFlagpoleLayer==NULL);

		CCRect TMXSpriteForCollision;
		CCSprite* pLandSprite = pLandLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pLandSprite!=NULL)
		{
			TMXSpriteForCollision = CCRectMake(convertToWorldSpace(pLandSprite->getPosition()).x,convertToWorldSpace(pLandSprite->getPosition()).y,getTileSize().width,getTileSize().height);
			if ( TMXSpriteForCollision.intersectsRect(HeroRect))
			{
				return enTileTypeLand;
			}
		}
		CCSprite* pBlockSprite = pBlockLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));
		if (pBlockSprite!=NULL)
		{
			TMXSpriteForCollision = CCRectMake(convertToWorldSpace(pBlockSprite->getPosition()).x,convertToWorldSpace(pBlockSprite->getPosition()).y,getTileSize().width,getTileSize().height);
			if (TMXSpriteForCollision.intersectsRect(HeroRect))
			{
				return enTileTypeBlock;
			}
		}
		CCSprite* pPipeSprite = pPipeLayer->tileAt(ccp(nHeroTilePosX,nHeroTilePosY));	
		if (pPipeSprite!=NULL)
		{
			TMXSpriteForCollision = CCRectMake(convertToWorldSpace(pPipeSprite->getPosition()).x,convertToWorldSpace(pPipeSprite->getPosition()).y,getTileSize().width,getTileSize().height);
			if (TMXSpriteForCollision.intersectsRect(HeroRect))
			{
				return enTileTypePipe;
			}
		}
		return enTileTypeNone;
	} while (false);
	CCLog("fun CMGameMap::HeroPosToTileType Error!");
	return enTileTypeNone;
}
