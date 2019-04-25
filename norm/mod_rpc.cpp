#include "stdafx.h"

#include "mod_rpc.h"
#include "discord_rpc.h"
#include <time.h>


static void handleDiscordReady(const DiscordUser* connectedUser)
{
    printf("\nDiscord: connected to user %s#%s - %s\n",
        connectedUser->username,
        connectedUser->discriminator,
        connectedUser->userId);
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
    printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}

static void handleDiscordError(int errcode, const char* message)
{
    printf("\nDiscord: error (%d: %s)\n", errcode, message);
}

static void handleDiscordJoin(const char* secret)
{
    printf("\nDiscord: join (%s)\n", secret);
}

static void handleDiscordSpectate(const char* secret)
{
    printf("\nDiscord: spectate (%s)\n", secret);
}

static void handleDiscordJoinRequest(const DiscordUser* request)
{
}

void rpc::updateDiscordPresence()
{
    if (this->SendPresence) {
        char buffer[256];
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));
        discordPresence.state = "West of House";
        sprintf_s(buffer, "Frustration level: %d", this->FrustrationLevel);
        discordPresence.details = buffer;
        discordPresence.startTimestamp = this->StartTime;
        discordPresence.endTimestamp = time(0) + 5 * 60;
        discordPresence.largeImageKey = "canary-large";
        discordPresence.smallImageKey = "ptb-small";
        discordPresence.partyId = "party1234";
        discordPresence.partySize = 1;
        discordPresence.partyMax = 6;
        discordPresence.matchSecret = "xyzzy";
        discordPresence.joinSecret = "join";
        discordPresence.spectateSecret = "look";
        discordPresence.instance = 0;
        Discord_UpdatePresence(&discordPresence);
    } else {
        Discord_ClearPresence();
    }
}


rpc::rpc(norm_dll::norm* c_state)
    : mod(c_state)
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.errored = handleDiscordError;
    handlers.joinGame = handleDiscordJoin;
    handlers.spectateGame = handleDiscordSpectate;
    handlers.joinRequest = handleDiscordJoinRequest;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}

rpc::~rpc()
{
}

#if ((CLIENT_VER <= 20180919 && CLIENT_VER >= 20180620) || CLIENT_VER_RE == 20180621)
int rpc::get_talk_type(void** this_obj, void** src, int* a1, int* a2, int* retval)
#elif CLIENT_VER == 20150000
int rpc::get_talk_type(void** this_obj, char** src, int* a1, char** a2, int* retval)
#endif
{
    if (strcmp((char*)*src, "/rpc") == 0) {
        updateDiscordPresence();
        char buf[64];
		sprintf_s(buf, "RPC executed!.");
        *retval = -1;
        return 1;
    }
    return 0;
}

void rpc::draw_scene(void* this_obj)
{
    Discord_RunCallbacks();
}
