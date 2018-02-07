#include "GameOverScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

CCScene* GameOverScene::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	GameOverScene *layer = GameOverScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameOverScene::init()
{
	if (!CCLayer::init())
	{
		return false;
	}

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

	CCMenuItemImage *exit = CCMenuItemImage::create(
		"exit.png",
		"exit.png",
		this,
		menu_selector(GameOverScene::exit));
	CCMenuItemImage *retry = CCMenuItemImage::create(
		"retry.png",
		"retry.png",
		this,
		menu_selector(GameOverScene::retry));

	exit->setPosition(ccp(visibleSize.width / 4, visibleSize.height / 4));
	retry->setPosition(ccp(visibleSize.width * 0.75f, visibleSize.height / 4));

	CCMenu* menu = CCMenu::create(exit, retry, NULL);
	menu->setPosition(CCPointZero);
	addChild(menu, 1);

	CCTexture2D* tex = CCTextureCache::sharedTextureCache()->addImage("./cards/card_back.png");

	CCSprite* bg = CCSprite::create("pokertable.jpg");
	bg->setPosition(ccp(0, 0));
	bg->setAnchorPoint(ccp(0, 0));

	this->addChild(bg);

	m_gameover = CCLabelTTF::create("You shouldn't be seeing this!", "Arial", 20);
	m_gameover->setPosition(ccp(visibleSize.width / 2, visibleSize.height / 2));
	addChild(m_gameover);
}

void GameOverScene::setWinner(bool winner)
{
	if (winner)
		m_gameover->setString("You won!");
	else m_gameover->setString("You lost!");
}

void GameOverScene::exit(CCObject* sender)
{
	CCDirector::sharedDirector()->end();
}

void GameOverScene::retry(CCObject* sender)
{
	CCScene *pScene = HelloWorld::scene();

	CCDirector::sharedDirector()->replaceScene(pScene);
}