#include "ice_network.h"
#include "juice/juice.h"
#include "message.h"

#include <SDL.h>
#include <SDL_log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

// All sdp should be this length
#define SDP_LENGTH JUICE_MAX_SDP_STRING_LEN
#define MAX_CANDIDATE 10

static int event_type_receive_message;

static int INITIAL_AGENT_LIST_CAPACITY = 10;

static juice_agent_t* create_agent(PlayerId player_id);

static void
on_state_changed(juice_agent_t* agent, juice_state_t state, void* user_ptr);

static void on_candidate(juice_agent_t* agent, const char* sdp, void* user_ptr);

static void on_gathering_done(juice_agent_t* agent, void* user_ptr);

static void
on_recv(juice_agent_t* agent, const char* data, size_t size, void* user_ptr);

// strcut to acces to good index in player_list
typedef struct {
	int len;
	int cap;
	int* data;
} UseIndexList;

// dynamic array for player agent
typedef struct {
	juice_agent_t** agents;
	Uint32 cap;
} AgentList;

typedef struct {
	int cap;
	char** data;
} SdpList;

static UseIndexList* use_index_list = NULL;
static AgentList* agent_list = NULL;
static SdpList* sdp_list = NULL;

// nb_val is the initial length of the array
UseIndexList* UseIndexList_create(int nb_val) {
	UseIndexList* list = (UseIndexList*)malloc(sizeof(UseIndexList));
	list->cap = nb_val;
	list->len = 0;
	list->data = (int*)malloc(nb_val * sizeof(int));
	return list;
}

void UseIndexList_add(UseIndexList* list, PlayerId player_id) {
	if (list->cap == list->len) {
		list->data = (int*)realloc(list->data, (2 * list->cap) * sizeof(int));
		list->cap *= 2;
	}
	list->data[list->len] = player_id;
	list->len += 1;
}

static void UseIndexList_remove_index(UseIndexList* list, int i) {
	if (list->len == 1) {
		list->len = 0;
	} else {
		list->data[i] = list->data[list->len - 1];
		list->len -= 1;
	}
}

static void UseIndexList_remove(UseIndexList* list, PlayerId id) {
	for (int i = 0; i < list->len; i++) {
		if (list->data[i] == id) {
			UseIndexList_remove_index(list, i);
			return;
		}
	}
}

void UseIndexList_free(UseIndexList* list) {
	free(list->data);
	free(list);
}

void init_use_index_list(void) {
	use_index_list = UseIndexList_create(INITIAL_AGENT_LIST_CAPACITY);
}

bool exist_in_use_index_list(PlayerId id) {
	for (int i = 0; i < use_index_list->len; i++)
		if (use_index_list->data[i] == id)
			return true;
	return false;
}

// nb_val is the initial length of the array
SdpList* SdpList_create(int nb_val) {
	SdpList* list = (SdpList*)malloc(sizeof(SdpList));
	list->cap = nb_val;
	list->data = (char**)malloc(nb_val * sizeof(char*));
	return list;
}

void SdpList_add(SdpList* list, PlayerId player_id) {
	if (player_id >= (int)agent_list->cap) {
		list->cap *= player_id + 1;
		list->data = realloc(list->data, list->cap * sizeof(char*));
	}
	list->data[player_id] = (char*)malloc(MAX_MESSAGE_SIZE * sizeof(char));
}

void SdpList_free(SdpList* list) {
	free(list->data);
	free(list);
}

void init_sdp_list(void) {
	sdp_list = SdpList_create(INITIAL_AGENT_LIST_CAPACITY);
}

AgentList* AgentList_create(int nb_val) {
	AgentList* list = (AgentList*)malloc(sizeof(AgentList));
	list->cap = INITIAL_AGENT_LIST_CAPACITY;
	list->agents = (juice_agent_t**)malloc(nb_val * sizeof(juice_agent_t*));
	return list;
}

void AgentList_add(PlayerId player_id) {
	if (player_id >= (int)agent_list->cap) {
		agent_list->cap *= player_id + 1;
		agent_list->agents = realloc(agent_list->agents,
									 agent_list->cap * sizeof(juice_agent_t*));
	}

	agent_list->agents[player_id] = create_agent(player_id);
	UseIndexList_add(use_index_list, player_id);
}

void AgentList_remove(PlayerId id) { UseIndexList_remove(use_index_list, id); }

void AgentList_free(void) {
	free(agent_list->agents);
	free(agent_list);
}

void init_agent_list(void) {
	agent_list = AgentList_create(INITIAL_AGENT_LIST_CAPACITY);
}

static juice_agent_t* create_agent(PlayerId player_id) {
	// Agent 1: Create agent
	juice_set_log_level(JUICE_LOG_LEVEL_VERBOSE);
	juice_config_t config;
	memset(&config, 0, sizeof(config));

	// STUN server example
	config.stun_server_host = "stun.l.google.com";
	config.stun_server_port = 19302;

	config.cb_state_changed = on_state_changed;
	config.cb_candidate = on_candidate;
	config.cb_gathering_done = on_gathering_done;
	config.cb_recv = on_recv;
	int* id = (int*)malloc(sizeof(int));
	*id = player_id;
	config.user_ptr = id;
	return juice_create(&config);
}

char* new_connection(PlayerId id) {
	AgentList_add(id);
	return get_local_sdp(id);
}

char* get_local_sdp(PlayerId player_id) {
	char* sdp = (char*)malloc(JUICE_MAX_SDP_STRING_LEN);
	bzero(sdp, JUICE_MAX_SDP_STRING_LEN);
	sdp_list->data[player_id] = malloc(JUICE_MAX_SDP_STRING_LEN);
	juice_gather_candidates(agent_list->agents[player_id]);
	sleep(1); // increase this time if there is no candidate
	juice_get_local_description(
		agent_list->agents[player_id], sdp, JUICE_MAX_SDP_STRING_LEN);
	// printf("My fresh new sdp : \n%s\n", sdp);
	return sdp;
}

void sdp_parser(char* sdp,
				char sdp_candidates[MAX_CANDIDATE][JUICE_MAX_SDP_STRING_LEN],
				int* nb_candidates,
				char sdp_description[JUICE_MAX_SDP_STRING_LEN]) {
	char* p = sdp;
	char buffer[JUICE_MAX_SDP_STRING_LEN];
	bzero(buffer, JUICE_MAX_SDP_STRING_LEN);
	int ind = 0;
	int category = 0;
	while (*p) {
		buffer[ind] = *p;
		if (*p != '\n') {
			if (ind == 2) {
				if (!strncmp("candidate", p, 9))
					category = -1;
				else if (!strncmp("end-of-", p, 7))
					category = 1;
			}
			ind++;
		} else {
			// printf("buffer %s", buffer);
			buffer[ind + 1] = '\0';
			if (category == -1) {
				strcpy(sdp_candidates[*nb_candidates], buffer);
				(*nb_candidates)++;
			} else if (category == 0) {
				strcat(sdp_description, buffer);
			}
			category = 0;
			bzero(buffer, ind + 1);
			ind = 0;
		}
		++p;
	}
	// printf("sdp_description : %s\n", sdp_description);
	// printf("candidate :\n");
	// for (int i = 0; i < *nb_candidates; i++)
	// 	printf("%s", sdp_candidates[i]);
}

void add_remote_agent(PlayerId id, char* sdp) {
	/* TODO : code this corectly even if the order of a can change */
	char sdp_description[JUICE_MAX_SDP_STRING_LEN] = "";
	char sdp_candidates[MAX_CANDIDATE][JUICE_MAX_SDP_STRING_LEN] = {0};

	int nb_candidates = 0;

	sdp_parser(sdp, sdp_candidates, &nb_candidates, sdp_description);

	// printf("Setting the remote description :\n%s\n", sdp_description);

	juice_set_remote_description(agent_list->agents[id], sdp_description);

	for (int i = 0; i < nb_candidates; i++) {
		juice_add_remote_candidate(agent_list->agents[id], sdp_candidates[i]);
	}

	sleep(1);
}

void send_data(PlayerId id, char* data, int size) {
	juice_send(agent_list->agents[id], data, size);
}

void broadcast_data(char* data, int size) {
	for (int i = 0; i < use_index_list->len; i++)
		send_data(use_index_list->data[i], data, size);
}

void remove_agent(PlayerId id) {
	UseIndexList_remove(use_index_list, id);
	juice_destroy(agent_list->agents[id]);
}

// Agent 1: on state changed
static void
on_state_changed(juice_agent_t* agent, juice_state_t state, void* user_ptr) {
	int id = *(int*)user_ptr;
	printf("State %d : %s\n", id, juice_state_to_string(state));
}

// Agent 1: on local candidate gathered
static void
on_candidate(juice_agent_t* agent, const char* sdpc, void* user_ptr) {
	int id = *(int*)user_ptr;
	strcat(sdp_list->data[id], sdpc);
	strcat(sdp_list->data[id], "\n");
}

// Agent 1: on local candidates gathering done
static void on_gathering_done(juice_agent_t* agent, void* user_ptr) {
	int id = *(int*)user_ptr;
	printf("Gathering done %d\n", id);
}

void init_event_type(int event_receive_message) {
	event_type_receive_message = event_receive_message;
}

// Agent 1: on message received
static void
on_recv(juice_agent_t* agent, const char* data, size_t size, void* user_ptr) {
	// printf("I Receive this :\n%s", data);
	// We push the received message to the SDL event queue
	if (size == sizeof(Message)) {

		SDL_Event ev;
		SDL_memset(&ev, 0, sizeof(ev));
		ev.type = event_type_receive_message;
		ev.user.code = 0xbeef;

		Message* msg = malloc(sizeof(Message));
		memcpy(msg, data, sizeof(Message));
		ev.user.data1 = msg;
		SDL_PushEvent(&ev);
	} else {
		SDL_Log("Receive a Message with the wrong size");
	}
}
