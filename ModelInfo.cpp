

#include "StdAfx.h"
#include "ModelInfo.h"

//TODO:	Layer the model data in the correct order.
//		What if the player has steel toe boots and amuli? Amuli covers the STB!
void ModelInfo::MergeData(ModelInfo* pSrc, DWORD dwLayer)
{
	//if ( !pSrc->wBasePalette )
	//	return;

	if (!wBasePalette)
		wBasePalette = pSrc->wBasePalette;

	for (PaletteRList::iterator newb = pSrc->lPalettes.begin(); newb != pSrc->lPalettes.end(); newb++)
	{
		WORD bNewbID = newb->wPaletteID;
		BYTE bNewbStart = newb->bOffset;
		BYTE bNewbEnd = newb->bOffset + newb->bLength;

		for (PaletteRList::iterator i = lPalettes.begin(); i != lPalettes.end(); )
		{
			WORD wBobID = i->wPaletteID;
			BYTE bBobStart = i->bOffset;
			BYTE bBobEnd = i->bOffset + i->bLength;

			if (bBobStart < bNewbStart)
			{
				if (bBobEnd <= bNewbStart) {
					//These do not collide
				}
				else if (bBobEnd > bNewbEnd) {
					//Going to have to split the existing into 2
					PaletteRpl pr1(wBobID, bBobStart, bNewbStart - bBobStart);
					PaletteRpl pr2(wBobID, bNewbEnd, bBobEnd - (bNewbEnd));
					lPalettes.push_back(pr1);
					lPalettes.push_back(pr2);
				}
				else
				{
					//Need to cut off high-side of the existing
					PaletteRpl pr1(wBobID, bBobStart, bNewbStart - bBobStart);
					lPalettes.push_back(pr1);
				}
				i++;
				continue;
			}
			else //if ( bBobStart >= bNewbStart )
			{
				if (bBobStart >= bNewbEnd)
				{
					//These do not collide
					i++;
				}
				else if (bBobEnd <= bNewbEnd)
				{
					//The existing is completely overwritten
					i = lPalettes.erase(i);
				}
				else
				{
					//Need to cut off low-side of the existing
					PaletteRpl pr2(wBobID, bNewbEnd, bBobEnd - bNewbEnd);
					lPalettes.push_back(pr2);
					i++;
				}
				continue;
			}
		}
	}
	std::copy(pSrc->lPalettes.begin(), pSrc->lPalettes.end(), std::back_inserter(lPalettes));

	for (TextureRList::iterator newb = pSrc->lTextures.begin(); newb != pSrc->lTextures.end(); newb++)
	{
		BYTE bNewbIndex = newb->bIndex;

		for (TextureRList::iterator i = lTextures.begin(); i != lTextures.end(); )
		{
			if (bNewbIndex != i->bIndex)
				i++;
			else
				i = lTextures.erase(i);
		}
	}
	std::copy(pSrc->lTextures.begin(), pSrc->lTextures.end(), std::back_inserter(lTextures));

	for (ModelRList::iterator newb = pSrc->lModels.begin(); newb != pSrc->lModels.end(); newb++)
	{
		BYTE bNewbIndex = newb->bIndex;

		for (ModelRList::iterator i = lModels.begin(); i != lModels.end(); )
		{
			if (bNewbIndex != i->bIndex)
				i++;
			else
				i = lModels.erase(i);
		}
	}
	std::copy(pSrc->lModels.begin(), pSrc->lModels.end(), std::back_inserter(lModels));
}

NetFood *ModelInfo::NetData()
{
	NetFood* Poo = new NetFood;

	Poo->WriteBYTE(0x11);
	Poo->WriteBYTE((BYTE)lPalettes.size());
	Poo->WriteBYTE((BYTE)lTextures.size());
	Poo->WriteBYTE((BYTE)lModels.size());

	if (!lPalettes.empty())
	{
		Poo->WritePackedDWORD(wBasePalette);

		for (PaletteRList::iterator i = lPalettes.begin(); i != lPalettes.end(); i++)
		{
			Poo->WritePackedDWORD(i->wPaletteID);
			Poo->WriteBYTE(i->bOffset);
			Poo->WriteBYTE(i->bLength);
		}
	}
	if (!lTextures.empty())
	{
		for (TextureRList::iterator i = lTextures.begin(); i != lTextures.end(); i++)
		{
			Poo->WriteBYTE(i->bIndex);
			Poo->WritePackedDWORD(i->wOriginID);
			Poo->WritePackedDWORD(i->wTextureID);
		}
	}
	if (!lModels.empty())
	{
		for (ModelRList::iterator i = lModels.begin(); i != lModels.end(); i++)
		{
			Poo->WriteBYTE(i->bIndex);
			Poo->WritePackedDWORD(i->wModelID);
		}
	}

	Poo->Align();

	//OutputConsoleBytes( Poo->GetData(), Poo->GetSize() );
	return Poo;
}
