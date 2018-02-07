#pragma once

#include <cocos2d.h>

class GameOverScene : public cocos2d::CCLayer
{
public:
	virtual bool init();
	static cocos2d::CCScene* scene();

	void setWinner(bool winner);

	void exit(CCObject* sender);
	void retry(CCObject* sender);

	CREATE_FUNC(GameOverScene);
private:
	cocos2d::CCLabelTTF* m_gameover;
};