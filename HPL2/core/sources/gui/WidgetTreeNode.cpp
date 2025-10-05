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

#include "gui/WidgetTreeNode.h"

#include "math/Math.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"
#include "gui/WidgetButton.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetTreeNode::cWidgetTreeNode(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_Window,apSet, apSkin)
	{
		mfNodeHeight = 0;
		mfNodeIndentation = 0;
	}

	//-----------------------------------------------------------------------

	cWidgetTreeNode::~cWidgetTreeNode()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::SetExtended(bool abX)
	{
		mbExtended = abX;
	}

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::SetSelected(bool abX, bool abDoCallbacks)
	{
		mbSelected = abX;
	}

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::SetNodeWidth(float afNodeWidth)
	{
		SetSize(cVector2f(afNodeWidth, 18));
	}

	float cWidgetTreeNode::GetNodeWidth()
	{
		return mvSize.x;
	}

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::AddChildNode(cWidgetTreeNode* apChildNode)
	{
		mvChildNodes.push_back(apChildNode);
		apChildNode->SetParentNode(this);

		mbHasChildren = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::RemoveChildNode(cWidgetTreeNode* apNode, bool abDelete)
	{
		apNode->SetParentNode(NULL);

		tWidgetTreeNodeVecIt it = mvChildNodes.begin();
		for (int i = 0; it != mvChildNodes.end(); ++it, ++i)
		{
			if (*it == apNode)
			{
				if(abDelete) hplDelete(*it);
				mvChildNodes.erase(it);
				break;
			}
		}

		mbHasChildren = !mvChildNodes.empty();
	}

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::DrawNode()
	{
		mfNodeHeight = 0;

		cVector3f vPos = GetGlobalPosition();

		if(mpParentNode)
		{
			mfNodeIndentation = mpParentNode->GetNodeIndentation()+14;
			SetGlobalPosition(mpParentNode->GetGlobalPosition() + cVector3f(0, mpParentNode->GetNodeHeight(), 0));
		}

		if(mbSelected)
		{
			mpSet->DrawGfx(mpGfxBackground, vPos + cVector3f(mfNodeIndentation + 16, 0, 0),
				mvSize, cColor(0.438f, 0.598f, 1, 1));
		}
		if (mbFocused)
		{
			mpSet->DrawGfx(mpGfxBackground, vPos,
				mvSize, cColor(0.6, 0.6, 0.6, 0.5));
		}

		if(mbHasChildren)
		{
			DrawBordersAndCorners(mpGfxButtonBackground, mvGfxButtonBorders, mvGfxButtonCorners,
				vPos + cVector3f(mfNodeIndentation+2,3,0.1f),
				cVector2f(12,12));

			DrawDefaultText(mbExtended ? _W("-") : _W("+"), vPos + cVector3f(mfNodeIndentation + 8, 1, 0.2f), eFontAlign_Center);
		}

		mpSet->DrawFont(msText, mpDefaultFontType, vPos + cVector3f(mfNodeIndentation + 20, 2, 0.1f), mvDefaultFontSize * 0.875f, mDefaultFontColor * mColorMul, eFontAlign_Left);
		mfNodeHeight += 18;

		if (mbExtended == false) return;

		for (int i = 0; i < (int)mvChildNodes.size(); ++i)
		{
			cWidgetTreeNode* pItem = mvChildNodes[i];
			if (pItem == NULL)
				continue;

			pItem->DrawNode();
			mfNodeHeight += pItem->GetNodeHeight();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetTreeNode::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		if (mpParentNode) return;
		DrawNode();
	}

	void cWidgetTreeNode::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_FrameBackground);
		mpGfxButtonBackground = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBackground);

		mvGfxButtonBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderRight);
		mvGfxButtonBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderLeft);
		mvGfxButtonBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderUp);
		mvGfxButtonBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderDown);

		mvGfxButtonCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLU);
		mvGfxButtonCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRU);
		mvGfxButtonCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRD);
		mvGfxButtonCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLD);
	}
}
