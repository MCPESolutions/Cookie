#include <SkinGameData.h>
#include<SkinUtil.h>
#include "../SDK/GlobalInstance.h"

void SkinGameData::updatePlayerList(std::unordered_map<mce::UUID, PlayerListEntry>* list) {
    if(!list || list->empty()) {
        return;
    }
    
    for(auto& [uuid, entry] : *list) {
        auto s = entry.skin;
        auto skin = entry.skin.serializedSkin;

        if(!skin)
            continue;
            
        auto& img = skin->mSkinImage;
        if(!img.mImageBytes.mBlob || img.mWidth <= 0 || img.mHeight <= 0)
            continue;

        int width = img.mWidth;
        int height = img.mHeight;  // �޸���ʹ����ȷ�ĸ߶�
        
        //GI::DisplayClientMessage("SkinData: %dx%d for player %s", width, height, entry.name.c_str());

        // ��ȡ����Ƥ������
        auto fullSkinData = SkinUtils::getFullSkinData(img);
        
        // ʹ�øĽ���ͷ����ȡ����
        auto croppedHead = SkinUtils::cropHeadImproved(img, true); // ����ñ�Ӳ�
        
        // ����Ľ�����ʧ�ܣ�����ԭ����
        if(croppedHead.empty()) {
            int type = (width == 64 && height == 64) ? 0 : 1;
            croppedHead = SkinUtils::cropHead(img, type, true);
        }
        
        // ����ͷ����64x64��ʾ�ߴ�
        std::vector<unsigned char> resizedHead;
        if(!croppedHead.empty()) {
            int headSize = (width == 64) ? 8 : 16;
            float scaleRatio = 64.0f / headSize;
            resizedHead = SkinUtils::resizeHead(croppedHead, headSize, headSize, scaleRatio);
        }

        if(!fullSkinData.empty()) {
            // �洢����Ƥ������
            fullskinmap[entry.name] = fullSkinData;

            if(!resizedHead.empty()) {
                skinmap[entry.name] = resizedHead;
            }
            playerlist[entry.name] = PlayerEntry(
                entry.name, skin, entry.xuid, entry.mId, 
                resizedHead, fullSkinData, width, height, entry.uuid
            );
            
     
        } else {
        }
    }
}