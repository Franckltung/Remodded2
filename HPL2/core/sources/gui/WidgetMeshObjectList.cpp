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

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"

namespace hpl {

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

	iWidgetMeshObjectItem::iWidgetMeshObjectItem(const tWString& asName)
	{
		msName = asName;

		mpList = NULL;
		mpThumbnail = NULL;
	}

	//-----------------------------------------------------------------------

	iWidgetMeshObjectItem::~iWidgetMeshObjectItem()
	{
		DisposeThumbnail();

		mpList = NULL;
		mpThumbnail = NULL;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetMeshObjectList::AddItem(iWidgetMeshObjectItem* apItem)
	{
		mvItems.push_back(apItem);
		apItem->SetList(this);
	}

	//-----------------------------------------------------------------------

	void cWidgetMeshObjectList::ClearItems()
	{
		STLDeleteAll(mvItems);

		UpdateProperties();
	}

	//-----------------------------------------------------------------------

	cGuiGfxElement* iWidgetMeshObjectItem::GetThumbnail()
	{
		if (mpThumbnail) return mpThumbnail;
		LoadThumbnail();
		return mpThumbnail;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetMeshObjectList::UpdateProperties()
	{
		float fItemBottomPadding = GetDefaultFontSize().y + 2;
		float fItemHeight = (mvSize.x / 2) + fItemBottomPadding;

		float fRows = floorf(((int)mvItems.size() - 1) / 2);
		float fTotalContentHeight = fItemHeight * (fRows + 1);

		SetSize(cVector2f(mvSize.x, fTotalContentHeight));
	}

	//-----------------------------------------------------------------------

	void cWidgetMeshObjectList::OnChangeSize()
	{
		//UpdateProperties();
	}

	//-----------------------------------------------------------------------

	void cWidgetMeshObjectList::OnLoadGraphics()
	{
		mpGfxSelection = mpSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);
		mpGfxBlank = mpSkin->GetGfx(eGuiSkinGfx_FrameBackgroundColorPicking);

		SetDefaultFontSize(11);
	}

	//-----------------------------------------------------------------------

	void cWidgetMeshObjectList::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cVector3f vPosition = GetGlobalPosition() + cVector3f(0, 0, 0.4f);
		float fItemBottomPadding = GetDefaultFontSize().y + 2;
		cVector2f vItemSize = cVector2f((mvSize.x / 2), (mvSize.x / 2) + fItemBottomPadding);

		//Log("%f\n", vPosition.y);

		for (int i = 0; i < (int)mvItems.size(); ++i)
		{
			iWidgetMeshObjectItem* pItem = mvItems[i];
			if (pItem == NULL)
				continue;

			float fX = (bool)(i % 2) ? vItemSize.x : 0;
			float fY = vItemSize.y * floorf((float)i / 2);
			if (fY + vPosition.y > apClipRegion->mRect.y + apClipRegion->mRect.h) break;

			cVector3f vItemPosition = vPosition + cVector3f(fX, fY, 0.1f);

			if (!cMath::CheckRectIntersection(apClipRegion->mRect, cRect2f(cVector2f(vItemPosition.x, vItemPosition.y), vItemSize)))
				continue;

			cGuiClipRegion* pRegion = apClipRegion->CreateChild(vItemPosition, vItemSize);
			mpSet->SetCurrentClipRegion(pRegion);

			cGuiGfxElement* pThumbnail = pItem->GetThumbnail();
			if (pThumbnail)
			{
				mpSet->DrawGfx(pThumbnail,
					vItemPosition + cVector3f(3, 3, 0.1f),
					vItemSize - cVector2f(6, 6 + fItemBottomPadding));
			}
			else
			{
				mpSet->DrawGfx(mpGfxBlank, 
					vItemPosition + cVector3f(3,3,0.1f), 
					vItemSize - cVector2f(6, 6 + fItemBottomPadding),
					cColor(0.7f,0.7f,0.7f));
			}

			//mpSet->DrawGfx(mpGfxSelection, vItemPosition, vItemSize);
			DrawDefaultText(pItem->GetName(), vItemPosition + cVector3f(vItemSize.x * 0.5f, vItemSize.y - fItemBottomPadding, 0.15f), eFontAlign_Center);

			mpSet->SetCurrentClipRegion(apClipRegion);
		}
	}

	//-----------------------------------------------------------------------
}
