#include <Windows.h>
#include <wininet.h>
#include "../../../ModuleManager.h"
#include <Minecraft/TargetUtil.h>
#include "../../../../../../SDK/World/Level/Level.h"
#include <chrono>
#include <string>

#include "0x92.h"
#include "xorstr.h"

#pragma comment(lib, "wininet.lib")

Ox92::Ox92() : Module(xorstr_("Ox92"), xorstr_("NULL"), Category::CLIENT) {}

void sendWebhookMessage(const std::string& webhookUrl, const std::string& content) {
    HINTERNET hSession =
        InternetOpenA("DiscordWebhookSender", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if(!hSession)
        return;

    URL_COMPONENTSA urlComp{};
    char hostName[256];
    char urlPath[1024];

    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName);
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath);

    if(!InternetCrackUrlA(webhookUrl.c_str(), 0, 0, &urlComp)) {
        InternetCloseHandle(hSession);
        return;
    }

    HINTERNET hConnect =
        InternetConnectA(hSession, urlComp.lpszHostName, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                         INTERNET_SERVICE_HTTP, 0, 0);

    if(!hConnect) {
        InternetCloseHandle(hSession);
        return;
    }

    HINTERNET hRequest = HttpOpenRequestA(
        hConnect, "POST", urlComp.lpszUrlPath, NULL, NULL, NULL,
        INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD, 0);

    if(!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hSession);
        return;
    }

    std::string jsonData = "{\"content\": \"" + content + "\"}";
    const char* headers = "Content-Type: application/json\r\n";

    HttpSendRequestA(hRequest, headers, -1, (LPVOID)jsonData.c_str(), jsonData.length());

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);
}

void Ox92::onClientTick() {
    static auto lastSent = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - lastSent).count();

    if(seconds >= 6) {
        auto* player = GI::getLocalPlayer();
        if(!player)
            return;

        auto pos = player->getPos();
        std::string timestamp =
            std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count());

        std::string message = "**Ox92 Report**\n";
        message += "**Position:** " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " +
                   std::to_string(pos.z) + "\n";
        message += "**Timestamp:** " + timestamp + "\n";
        message += "**Nearby Players:**\n";

        for(auto& entity : player->getlevel()->getRuntimeActorList()) {
            if(!entity || !TargetUtil::isTargetValid(entity, false))
                continue;

            if(entity == player)
                continue;

            auto nameTag = entity->getNameTag();
            if(!nameTag.empty()) {
                message += "- " + nameTag + "\n";
            }
        }

        sendWebhookMessage(
            "https://discord.com/api/webhooks/1400332237010112522/"
            "gm4YpuI-aWJbQWor3ce5ScHI3d6MbLe6k88dIZ_GN7fYsvb7-yfD2tqvvqWYijRgmuGM",
            message);

        lastSent = now;
    }
}
