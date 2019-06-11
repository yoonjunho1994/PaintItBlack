#include "stdafx.h"

#include "discord_rpc.h"
#include "mod_rpc.h"
#include "norm.h"

#include <time.h>
#include <sstream>


static void handleDiscordReady(const DiscordUser* connectedUser)
{
	//char buf[256];
    //c_state->dbg_sock->do_send("rpc init start!");
    //sprintf_s(buf, "handleDiscordReady: connected to user %s#%s - %s\n",
    //    connectedUser->username,
    //    connectedUser->discriminator,
    //    connectedUser->userId);
    //c_state->dbg_sock->do_send("rpc init start!");
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
    //c_state->dbg_sock->do_send("handleDiscordDisconnected");
}

static void handleDiscordError(int errcode, const char* message)
{
    //c_state->dbg_sock->do_send("handleDiscordError");
}

static void handleDiscordJoin(const char* secret)
{
    //c_state->dbg_sock->do_send("handleDiscordJoin");
}

static void handleDiscordSpectate(const char* secret)
{
    //c_state->dbg_sock->do_send("handleDiscordSpectate");
}

static void handleDiscordJoinRequest(const DiscordUser* request)
{
}

void rpc::updateDiscordPresence()
{
    if (this->SendPresence) {
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));

        char state_buffer[256];
		sprintf_s(state_buffer, "Level: %d/%d", p_session.get_level(), p_session.get_joblevel());
		discordPresence.state = state_buffer;

		char name_buffer[256];
		sprintf_s(name_buffer, "IGN: %s", p_session.get_name());
        discordPresence.details = name_buffer;

        discordPresence.startTimestamp = this->StartTime;
		
		char large_img_buffer[256];
		sprintf_s(large_img_buffer, "%s_male", p_session.get_job_type().c_str());
        discordPresence.largeImageKey = large_img_buffer;

        discordPresence.smallImageKey = "marker";
		
		char large_img_txt_buffer[256];
        sprintf_s(large_img_txt_buffer, "Job: %s", p_session.get_job<std::string>().c_str());
		discordPresence.largeImageText = large_img_txt_buffer;

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
	this->StartTime = time(0);
}

int rpc::get_talk_type(char* src, int* retval)
{
    if (strcmp(src, "/rpc") == 0) {
		this->SendPresence ^= 1;
        char buf[64];
		sprintf_s(buf, "RPC executed!.");
		this->print_to_chat(buf);
        *retval = -1;
        return 1;
    }
    return 0;
}

void rpc::draw_scene(void* this_obj)
{
    Discord_RunCallbacks();
	updateDiscordPresence();
}
