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

#include "gui/WidgetNodeTree.h"

#include "math/Math.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetNodeTree::cWidgetNodeTree(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_NodeTree,apSet, apSkin)
	{
		mpSelectedNode = NULL;
	}

	cWidgetTreeNode::cWidgetTreeNode(cWidgetNodeTree* apNodeTree)
	{
		mpNodeTree = apNodeTree;

		mfNodeHeight = 0;
		mfNodeIndentation = 0;

		mpParentNode = NULL;
	}

	//-----------------------------------------------------------------------

	cWidgetNodeTree::~cWidgetNodeTree()
	{
		STLDeleteAll(mvNodes);
	}

	cWidgetTreeNode::~cWidgetTreeNode()
	{
		STLDeleteAll(mvChildNodes);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetNodeTree::SelectNode(cWidgetTreeNode* apNode, bool abDoCallbacks)
	{
		mpSelectedNode = apNode;
	}

	void cWidgetTreeNode::SetSelected(bool abX, bool abDoCallbacks)
	{
		mpNodeTree->SelectNode(this, abDoCallbacks);
	}

	//-----------------------------------------------------------------------

	void cWidgetNodeTree::AddChildNode(cWidgetTreeNode* apChildNode)
	{
		mvNodes.push_back(apChildNode);
	}

	void cWidgetTreeNode::AddChildNode(cWidgetTreeNode* apChildNode)
	{
		mvChildNodes.push_back(apChildNode);
		apChildNode->SetParentNode(this);
	}

	//-----------------------------------------------------------------------

	void cWidgetNodeTree::RemoveChildNode(cWidgetTreeNode* apNode, bool abDelete)
	{
		tWidgetTreeNodeVecIt it = mvNodes.begin();
		for (int i = 0; it != mvNodes.end(); ++it, ++i)
		{
			if (*it == apNode)
			{
				apNode->SetParentNode(NULL);
				if (abDelete) hplDelete(*it);
				mvNodes.erase(it);
				break;
			}
		}
	}

	void cWidgetTreeNode::RemoveChildNode(cWidgetTreeNode* apNode, bool abDelete)
	{
		tWidgetTreeNodeVecIt it = mvChildNodes.begin();
		for (int i = 0; it != mvChildNodes.end(); ++it, ++i)
		{
			if (*it == apNode)
			{
				apNode->SetParentNode(NULL);
				if (abDelete) hplDelete(*it);
				mvChildNodes.erase(it);
				break;
			}
		}
	}

	//-----------------------------------------------------------------------

	cWidgetTreeNode* cWidgetNodeTree::AddTreeNode(const tWString& asNode)
	{
		cWidgetTreeNode* pNode = hplNew(cWidgetTreeNode,(this));
		pNode->SetName(asNode);
		AddChildNode(pNode);

		return pNode;
	}

	cWidgetTreeNode* cWidgetTreeNode::AddTreeNode(const tWString& asNode)
	{
		cWidgetTreeNode* pNode = hplNew(cWidgetTreeNode, (mpNodeTree));
		pNode->SetName(asNode);
		AddChildNode(pNode);

		return pNode;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetNodeTree::DrawTreeNode(cWidgetTreeNode* apNode, const cVector3f avPos)
	{
		cVector3f vPos = cVector3f(avPos.x, avPos.y, avPos.z + 1);
		cVector2f vSize = cVector2f(mvSize.x, 16);

		if (apNode->GetParentNode())
		{
			apNode->SetNodeIndentation(apNode->GetParentNode()->GetNodeIndentation() + 11);
		}

		float fIndentation = apNode->GetNodeIndentation();

		/////////////////////////
		// Highlight stuff
		if(apNode->IsSelected())
		{
			mpSet->DrawGfx(mpGfxBackground, vPos,
				vSize, cColor(0.6, 0.6, 0.6, 0.5));
		}
		if(mpSelectedNode && mpSelectedNode == apNode)
		{
			mpSet->DrawGfx(mpGfxBackground, vPos + cVector3f(fIndentation + 12, 0, 0),
				vSize - cVector2f(fIndentation + 12, 0), cColor(0.438f, 0.598f, 1, 1));
		}

		tWidgetTreeNodeVec& vpChildren = apNode->GetChildNodes();

		/////////////////////////
		// Extend button
		if(vpChildren.empty()==false)
		{
			DrawBordersAndCorners(mpGfxButtonBackground, mvGfxButtonBorders, mvGfxButtonCorners,
				vPos + cVector3f(fIndentation, 3, 0.1f),
				cVector2f(10, 10));

			DrawDefaultText(apNode->IsExtended() ? _W("-") : _W("+"), vPos + cVector3f(fIndentation + 5, 0, 0.2f), eFontAlign_Center);
		}

		mpSet->DrawFont(apNode->GetName(), mpDefaultFontType, vPos + cVector3f(fIndentation + 17, 1.25f, 0.1f), mvDefaultFontSize * 0.875f, mDefaultFontColor * mColorMul, eFontAlign_Left);
		apNode->SetNodeHeight(16);

		if(apNode->IsExtended() == false) return;

		float fNodeHeight = 16;

		for (int i = 0; i < (int)vpChildren.size(); ++i)
		{
			cWidgetTreeNode* pItem = vpChildren[i];
			if (pItem == NULL)
				continue;

			DrawTreeNode(pItem, cVector3f(vPos.x, vPos.y + fNodeHeight, vPos.z));
			fNodeHeight += pItem->GetNodeHeight();
		}

		apNode->SetNodeHeight(fNodeHeight);
	}

	void cWidgetNodeTree::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cVector3f vPos = GetGlobalPosition();

		for (int i = 0; i < (int)mvNodes.size(); ++i)
		{
			cWidgetTreeNode* pItem = mvNodes[i];
			if (pItem == NULL)
				continue;

			DrawTreeNode(pItem, vPos);
			vPos.y += pItem->GetNodeHeight();
		}

		if(mvSize.y != vPos.y + 16)
			SetSize(cVector2f(mvSize.x, vPos.y + 16));
	}

	void cWidgetNodeTree::OnLoadGraphics()
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