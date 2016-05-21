#ifndef PAUSESTATE_H
#define PAUSESTATE_H

#include "BaseState.h"

#include <string>
#include <stack>

class GameObject;

class PauseState : public BaseState
{
    PauseState();
    ~PauseState();
public:
    void LoadState();
    void InitState();
    void Draw_State();
    void UnloadState();
    void FreeState();
    void UpdateState();
    static PauseState* getInstance();
private:
    enum Screens
    {
        S_MAIN,
        S_OPTIONS,
        S_MAINMENU,
        S_QUIT,
        S_TOTAL
    };

    typedef void (*StateFunc)(GameObject* obj, const std::string& textName);

    struct StateNode
    {
        StateNode();
        StateFunc func_;
        unsigned int screenType_;
        StateNode* parent_;
        Vector2 selectorPos_;
        std::list<StateNode*> child_;
    };

    typedef std::list<StateNode*> StateNodeList;
private:
    void deleteChildren_(std::list<StateNode*>& children);
private:
    GameObject* screen_;
    GameObject* selector_;
    StateNode* root_;
    StateNodeList::iterator currNodeIt_;
    StateNodeList* currChildList_;
    std::stack<std::pair<StateNodeList::iterator,StateNodeList*> > itChildListStack_;
    std::string screenTextureNames_[S_TOTAL];
};

#endif