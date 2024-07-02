#include "ItemSchema.h"

const StickerKit* ItemSchema::GetStickerKitDefinition(int iStickerKitID)
{
	int iIndex = stickerKits.find(iStickerKitID);
	return stickerKits.memory.memory[iIndex].value;
}
