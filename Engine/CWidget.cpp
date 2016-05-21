/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */


#include "stdafx.h"
#include "CWidget.h"

void CWidget::AddSubWidget( CWidget* widget )
{
	//set the widget's parent as me
	widget->gameObject->transform->SetParent(gameObject->transform);
}

void CWidget::RemoveSubWidget( CWidget* widget )
{
	widget->gameObject->transform->SetParent(0);
	mySubWidgets.erase(std::remove(mySubWidgets.begin(), mySubWidgets.end(), widget),mySubWidgets.end());
}

void CWidget::OnFree()
{
	Log("Deleting CWidget -- UNRESOLVED PARENT DELETION ISSUE");
}

meta_define(CWidget);

///////////////////////
//Nicholas Fuller