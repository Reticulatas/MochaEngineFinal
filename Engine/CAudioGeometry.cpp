#include "stdafx.h"
#include "CAudioGeometry.h"
#include "CMeshRenderer.h"
#include "GameObject.h"
#include "AudioManager.h"

CAudioGeometry::CAudioGeometry()
{
}

CAudioGeometry::~CAudioGeometry()
{
    //AudioManager::getInstance()->RemoveGeometry(this);
}

void CAudioGeometry::OnInit(void)
{
}

void CAudioGeometry::OnUpdate(void)
{
    //AudioManager::getInstance()->UpdateGeometry(this,gameObject->GetComponent<CMeshRenderer>()->GetMesh()->GetAABB());
}

void CAudioGeometry::OnFree(void)
{
    //AudioManager::getInstance()->RemoveGeometry(this);
}

meta_define(CAudioGeometry);