#include "HelloWorldScene.h"
#include "Table.h"
#include "Player.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		this,
		menu_selector(HelloWorld::menuCloseCallback));

	//CCMenuItemImage *bid = CCMenuItemImage::create(
	//	"bet.png",
	//	"bet.png",
	//	this,
	//	menu_selector(HelloWorld::bet));
	bidmore = CCMenuItemImage::create(
		"bet_more.png",
		"bet_more.png",
		this,
		menu_selector(HelloWorld::bet));
	bidless = CCMenuItemImage::create(
		"bet_minus.png",
		"bet_minus.png",
		this,
		menu_selector(HelloWorld::bet));
	CCMenuItemImage *fold = CCMenuItemImage::create(
		"fold.png",
		"fold.png",
		this,
		menu_selector(HelloWorld::fold));
	CCMenuItemImage *finish = CCMenuItemImage::create(
		"bet.png",
		"bet.png",
		this,
		menu_selector(HelloWorld::finish));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width / 2,
		origin.y + pCloseItem->getContentSize().height / 2));
	bidmore->setPosition(ccp(((origin.x + visibleSize.width) / 2) + bidmore->getContentSize().width,
		origin.y + bidmore->getContentSize().height));
	bidless->setPosition(ccp(((origin.x + visibleSize.width) / 2) - bidless->getContentSize().width,
		origin.y + bidmore->getContentSize().height));
	fold->setPosition(ccp((origin.x + visibleSize.width) / 2,
		20 + origin.y + bidmore->getContentSize().height * 2));
	finish->setPosition(ccp(((origin.x + visibleSize.width) / 2) + finish->getContentSize().width + 50,
		origin.y + bidmore->getContentSize().height));

    // create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, bidmore, bidless, fold, finish, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

	CCTexture2D* tex = CCTextureCache::sharedTextureCache()->addImage("./cards/card_back.png");

	CCSprite* bg = CCSprite::create("pokertable.jpg");
	bg->setPosition(ccp(0, 0));
	bg->setAnchorPoint(ccp(0, 0));

	this->addChild(bg);

	m_controllingPlayer = new Player();
	m_agent = new Player();
	m_table = new Table(m_controllingPlayer, m_agent, this);
    
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();
}

void HelloWorld::bet(CCObject* pSender)
{
	if (pSender == bidmore)
		m_table->bet(10);
	else if (pSender == bidless)
		m_table->bet(-10);
}

void HelloWorld::fold(CCObject* pSender)
{
	m_table->fold(true);
}

void HelloWorld::finish(CCObject* pSender)
{
	m_table->betting();
}