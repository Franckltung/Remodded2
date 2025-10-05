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

#ifndef HPL_WIDGET_TREE_NODE_H
#define HPL_WIDGET_TREE_NODE_H

#include "gui/Widget.h"

namespace hpl {

	class cWidgetTreeNode;
	class cWidgetButton;

	typedef std::vector<cWidgetTreeNode*> tWidgetTreeNodeVec;
	typedef tWidgetTreeNodeVec::iterator tWidgetTreeNodeVecIt;

	class cWidgetTreeNode : public iWidget
	{
	public:
		cWidgetTreeNode(cGuiSet *apSet, cGuiSkin *apSkin);
		virtual ~cWidgetTreeNode();

		void SetExtended(bool abX);
		bool IsExtended() { return mbExtended; }

		void SetSelected(bool abX, bool abDoCallbacks = false);
		bool IsSelected() { return mbSelected; }

		void SetParentNode(cWidgetTreeNode* apNode) { mpParentNode = apNode; }
		cWidgetTreeNode* GetParentNode() { return mpParentNode; }

		float GetNodeHeight() { return mfNodeHeight; }
		float GetNodeIndentation() { return mfNodeIndentation; }

		void SetNodeWidth(float afNodeWitdth);
		float GetNodeWidth();

		void AddChildNode(cWidgetTreeNode* apChildNode);
		void RemoveChildNode(cWidgetTreeNode* apNode, bool abDelete = false);

		void DrawNode();

	protected:
		/////////////////////////
		// Implemented functions
		void OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion);

		void OnLoadGraphics();

		/////////////////////////
		// Data
		cWidgetTreeNode* mpParentNode;
		tWidgetTreeNodeVec mvChildNodes;

		cGuiGfxElement* mpGfxBackground;
		cGuiGfxElement* mpGfxButtonBackground;
		cGuiGfxElement* mvGfxButtonBorders[4];
		cGuiGfxElement* mvGfxButtonCorners[4];

		bool mbExtended;
		bool mbSelected;
		bool mbHighlighted;
		bool mbHasChildren;

		float mfNodeHeight;
		float mfNodeIndentation;
	};

};
#endif // HPL_WIDGET_TREE_NODE_H
