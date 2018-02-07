#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class Table;
class Player;

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

	void bet(CCObject* pSender);
	void fold(CCObject* pSender);
	void finish(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

private:
	Player* m_controllingPlayer;
	Player* m_agent;
	Table* m_table;

	cocos2d::CCMenuItemImage *bidmore;
	cocos2d::CCMenuItemImage *bidless;
};

#endif // __HELLOWORLD_SCENE_H__
