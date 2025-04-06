#include "message.h"
#include "player.h"

#define MAX_MESSAGE_SIZE 4096

void init_use_index_list(void);

void init_sdp_list(void);

void init_agent_list(void);

void init_event_type(int event_receive_message);

char* new_connection(PlayerId id);

// char* get_local_sdp(PlayerId player_id);

bool exist_in_use_index_list(PlayerId id);

char* get_local_sdp(PlayerId player_id);

void add_remote_agent(PlayerId id, char* sdp);

void remove_agent(PlayerId id);

void send_data(PlayerId id, char* data, int size);

void broadcast_data(char* data, int size);