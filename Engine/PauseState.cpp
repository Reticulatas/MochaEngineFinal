#include "stdafx.h"
#include "PauseState.h"
#include "ObjectFactory.h"
#include "GameObject.h"
#include "CMeshRenderer.h"
#include "TextureNode.h"
#include "Input.h"
#include "GSM.h"

void Resume(GameObject*, const std::string&)
{
    GSM::getInstance()->PopState();
}

void HowToPlay(GameObject*, const std::string&)
{
    GSM::getInstance()->PopState();
}

void MainMenu(GameObject*,const std::string&)
{
    GSM::getInstance()->PopState();
}

void Quit(GameObject*,const std::string&)
{
    GSM::getInstance()->PopState();
}

void SetScreen(GameObject* obj, const std::string& textName)
{
    CMeshRenderer* rend = obj->GetComponent<CMeshRenderer>();
    TextureNode* tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
    tNode->SetTexture(textName);
}

PauseState::PauseState()
{
    screenTextureNames_[S_MAIN] = "pauseMenu.png";
    screenTextureNames_[S_OPTIONS] = "pauseMenu.png";
    screenTextureNames_[S_MAINMENU] = "pauseMenu_MainMenu.png";
    screenTextureNames_[S_QUIT] = "pauseMenu_Quit.png";
}

PauseState::~PauseState()
{
}

void PauseState::LoadState()
{
    GameObject* light = ObjectFactory::getInstance()->MakeNewGameObject("Light",this);
    CLight* clight = light->AddComponent<CLight>();
    clight->SetLightType(CLight::POINT);
    clight->SetRange(9);
    clight->SetDiffuse(COLOR::WHITE);
    clight->SetAtt(3.5f);
    clight->mInnerAngle = 20;
    clight->mOuterAngle = 50;
    clight->SetSpot(5);

    screen_ = ObjectFactory::getInstance()->MakeNewGameObject("PauseScreen",this);
    screen_->transform->SetPosition(0.0f,0.0f,0.0f);
    screen_->transform->scale.x = 0.36f;
    screen_->transform->scale.y = 0.2f;
    screen_->transform->scale.z = 0.5f;

    CMeshRenderer* rend = screen_->AddComponent<CMeshRenderer>();
    rend->SetMesh(AssetManager::getInstance()->GetAsset("Quad.bean"));

    TextureNode* tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
    tNode->SetTexture("pauseMenu.png");

    selector_ = ObjectFactory::getInstance()->MakeNewGameObject("Selector",this);
    selector_->transform->SetPosition(0.0f,0.0f,0.1f);
    selector_->transform->scale.x = 0.01f;
    selector_->transform->scale.y = 0.01f;
    selector_->transform->scale.z = 0.5f;

    rend = selector_->AddComponent<CMeshRenderer>();
    rend->SetMesh(AssetManager::getInstance()->GetAsset("Quad.bean"));

    tNode = dynamic_cast<TextureNode*>(rend->GetMaterial()->GetAttribute<TextureNode>(0));
    tNode->SetTexture("menuPointer.png");

    // Main Camera
    GameObject* cam = ObjectFactory::getInstance()->MakeNewGameObject("Camera",this);
    CCamera* c = cam->AddComponent<CCamera>();
    cam->transform->SetPosition(0,0,1.0f);
    cam->transform->LookAt(&screen_->transform->GetPosition());
    c->mActiveProj = CCamera::ORTHO;
    c->mCamType = CCamera::MAIN;

    // Oculus Camera
    cam = ObjectFactory::getInstance()->MakeNewGameObject("Camera",this);
    c = cam->AddComponent<CCamera>();
    cam->transform->SetPosition(0,0,1.0f);
    cam->transform->LookAt(&screen_->transform->GetPosition());
    c->mCamType = CCamera::VR;
    c->SetRenderTarget(AssetManager::getInstance()->GetAsset("SecondaryRT.mrt"));
    
    root_ = new StateNode();
    
    StateNode* resume = new StateNode();
    StateNode* howToPlay = new StateNode();
    StateNode* options = new StateNode();
    StateNode* mainMenu = new StateNode();
    StateNode* quit = new StateNode();
    
    root_->child_.push_back(resume);
    root_->child_.push_back(howToPlay);
    root_->child_.push_back(options);
    root_->child_.push_back(mainMenu);
    root_->child_.push_back(quit);

    // Resume
    resume->parent_ = root_;
    resume->func_ = &Resume;
    resume->selectorPos_ = Vector2(0.16f,0.042f);

    // How to play
    howToPlay->parent_ = root_;
    howToPlay->func_ = &HowToPlay;
    howToPlay->selectorPos_ = Vector2(0.14f,0.032f);

    // Options
    options->parent_ = root_;
    options->screenType_ = S_OPTIONS;
    options->func_ = &Resume;
    options->child_;
    options->selectorPos_ = Vector2(0.14f,0.022f);

    // Main Menu
    mainMenu->parent_ = root_;
    mainMenu->screenType_ = S_MAINMENU;
    mainMenu->func_ = &SetScreen;
    mainMenu->selectorPos_ = Vector2(0.14f,-0.012f);

    // Main Menu > Yes
    StateNode* node = new StateNode();
    node->parent_ = mainMenu;
    node->func_ = &MainMenu;
    mainMenu->child_.push_back(node);
    node->selectorPos_ = Vector2(0.11f,-0.03f);

    // Main Menu > No
    node = new StateNode();
    node->parent_ = mainMenu;
    node->screenType_ = S_MAIN;
    node->func_ = &SetScreen;
    mainMenu->child_.push_back(node);
    node->selectorPos_ = Vector2(0.11f,-0.04f);

    // Quit
    quit->parent_ = root_;
    quit->screenType_ = S_QUIT;
    quit->func_ = &SetScreen;
    quit->selectorPos_ = Vector2(0.14f,-0.045f);
    
    // Quit > Yes
    node = new StateNode();
    node->parent_ = quit;
    node->func_ = &Quit;
    quit->child_.push_back(node);
    node->selectorPos_ = Vector2(0.11f,-0.063f);

    // Quit > No
    node = new StateNode();
    node->parent_ = quit;
    node->screenType_ = S_MAIN;
    node->func_ = &SetScreen;
    quit->child_.push_back(node);
    node->selectorPos_ = Vector2(0.11f,-0.073f);
    
    currChildList_  = &root_->child_;
    currNodeIt_ = currChildList_->begin();
}

void PauseState::InitState()
{ 
    currChildList_ = &root_->child_;
    currNodeIt_ = currChildList_->begin();
}

void PauseState::Draw_State()
{
    BaseState::Draw_State();
}

void PauseState::UnloadState()
{
    deleteChildren_(root_->child_);
    delete root_;
}

void PauseState::FreeState()
{

}

void PauseState::UpdateState()
{
    BaseState::UpdateState();
    
    if(Input::GetTriggered(0,"up"))
    {
        if(currNodeIt_ == currChildList_->begin())
            currNodeIt_ = currChildList_->end();
        --currNodeIt_;
    }       
    else if(Input::GetTriggered(0,"down"))
    {
        if(currNodeIt_ == --currChildList_->end())
            currNodeIt_ = currChildList_->begin();
        else
            ++currNodeIt_;
    }
    else if(Input::GetTriggered(0,"select"))
    {
        itChildListStack_.push(std::pair<StateNodeList::iterator,StateNodeList*>(currNodeIt_,currChildList_));
    }
    selector_->transform->SetPositionX((*currNodeIt_)->selectorPos_.x);
    selector_->transform->SetPositionY((*currNodeIt_)->selectorPos_.y);
}

PauseState* PauseState::getInstance()
{
    static PauseState* instance = 0;

    if(!instance)
        instance = new PauseState();

    return instance;
}

//-----------------------------------------------------------------------------

void PauseState::deleteChildren_(std::list<StateNode*>& children)
{
    std::list<StateNode*>::iterator nodeIt = children.begin();
    while(nodeIt != children.end())
    {
        deleteChildren_((*nodeIt)->child_);
        delete *nodeIt;
        ++nodeIt;
    }
    children.clear();
}

//-----------------------------------------------------------------------------

PauseState::StateNode::StateNode()
    :   func_(0),
        parent_(0)
{
}
