#include "stdafx.h"

#include "discord_rpc.h"
#include "mod_rpc.h"
#include <time.h>

static void handleDiscordReady(const DiscordUser* connectedUser)
{
    MessageBoxA(0, "handleDiscordReady", "norm.dll error!", MB_OK);
    //printf("\nDiscord: connected to user %s#%s - %s\n",
     //   connectedUser->username,
       // connectedUser->discriminator,
      //  connectedUser->userId);
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
    MessageBoxA(0, "handleDiscordDisconnected", "norm.dll error!", MB_OK);
    //printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}

static void handleDiscordError(int errcode, const char* message)
{
    MessageBoxA(0, "handleDiscordError", "norm.dll error!", MB_OK);
    //printf("\nDiscord: error (%d: %s)\n", errcode, message);
}

static void handleDiscordJoin(const char* secret)
{
    MessageBoxA(0, "handleDiscordJoin", "norm.dll error!", MB_OK);
    //("\nDiscord: join (%s)\n", secret);
}

static void handleDiscordSpectate(const char* secret)
{
    MessageBoxA(0, "handleDiscordSpectate", "norm.dll error!", MB_OK);
    //printf("\nDiscord: spectate (%s)\n", secret);
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
        discordPresence.state = "Level: 99/70";
        sprintf_s(buffer, "IGN: norm");
        discordPresence.details = buffer;
        discordPresence.startTimestamp = this->StartTime;
        discordPresence.largeImageKey = "novice";
        discordPresence.smallImageKey = "marker";
        discordPresence.largeImageText = "Job: Crusader";
        discordPresence.smallImageText = "Dont know what to put here!";
        discordPresence.instance = 0;
        Discord_UpdatePresence(&discordPresence);
    } else {
        Discord_ClearPresence();
    }
}

rpc::rpc(norm_dll::norm* c_state)
    : mod(c_state)
{
}

rpc::~rpc()
{
}

void rpc::init() {
    c_state->dbg_sock->do_send("rpc init start!");
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.errored = handleDiscordError;
    handlers.joinGame = handleDiscordJoin;
    handlers.spectateGame = handleDiscordSpectate;
    handlers.joinRequest = handleDiscordJoinRequest;
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

    StartTime = time(0);
    c_state->dbg_sock->do_send("rpc init done!");
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