/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "gui/WidgetMeshObjectList.h"


namespace hpl {

	/*
	
	cEditorObjectList::cEditorObjectList(cEditorWindowObjectBrowser* apBrowser, cGuiSet* apSet, cGuiSkin* apSkin) : iWidget(eWidgetType_ListBox, apSet, apSkin)
	{
		mpBrowser = apBrowser;
	}

	cEditorObjectList::~cEditorObjectList()
	{
	}

	void cEditorObjectList::UpdateProperties()
	{
		float fItemHeight = (mvSize.x * 0.5);

		float fTotalContentHeight = fItemHeight * floorf(((int)mvItems.size()-1) * 0.5f);

		SetSize(cVector2f(mvSize.x, fTotalContentHeight));
	}

	void cEditorObjectList::OnLoadGraphics()
	{
		mpGfxSelection = mpSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);
	}

	void cEditorObjectList::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cVector3f vPosition = GetGlobalPosition() + cVector3f(0, 0, 0.4f);
		cVector2f vItemSize = cVector2f((mvSize.x * 0.5),(mvSize.x * 0.5));

		for (int i = 0; i < (int)mvItems.size(); ++i)
		{
			cWidgetItem* pItem = mvItems[i];
			if (pItem == NULL)
				continue;

			float fX = (bool)(i % 1) ? vItemSize.x : 0;
			float fY = vItemSize.y * floorf(i * 0.5f);

			cVector3f vItemPosition = vPosition + cVector3f(fX, fY, 0.1f);

			if (!cMath::CheckRectIntersection(apClipRegion->mRect, cRect2f(cVector2f(vItemPosition.x, vItemPosition.y), vItemSize)))
				return;

			cGuiClipRegion* pRegion = apClipRegion->CreateChild(vItemPosition, vItemSize);
			mpSet->SetCurrentClipRegion(pRegion);

			mpSet->DrawGfx(mpGfxSelection, vItemPosition, vItemSize, cColor(1, 0, 0, 1));

			if (pItem->IsSelected())
			{
				mpSet->DrawGfx(mpGfxSelection, vItemPosition - cVector3f(0, 0, 0.01f), vItemSize);
			}

			mpSet->SetCurrentClipRegion(apClipRegion);
		}
	}

	*/

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetMeshObjectList::cWidgetMeshObjectList(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_MeshObjectList,apSet, apSkin)
	{
	}

	//-----------------------------------------------------------------------

	cWidgetMeshObjectList::~cWidgetMeshObjectList()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
}
