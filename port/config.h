#ifndef __CONFIG_H
#define __CONFIG_H

#include "defineValues.h"

typedef struct {
	int x;
	int y;
	int set;
} Location;

class Config {
public:
	Config( );
	~Config();
	void initConfig(int valid_license, const char *config_file);
	void setConfig(int devN, char *devName);
	void setDevice(int devN, char *devName);
	int readConfig();
	int writeConfig();
	int readGlobalConfig( char *dir );
	int writeGlobalConfig();

	char my_name[LAB_LEN];
	char session_file[FILE_LEN];
	char session_dir[FILE_LEN];
	char config_file[FILE_LEN];
	char global_config_file[FILE_LEN];
	char lab_id[LAB_LEN];
	char listener_id[LAB_LEN];
	char session_id[LAB_LEN];
	int dev_idx;
	char dev_name[LAB_LEN];
	int transition_mode;
	int lock_options;

	/* options */
	int random_trials;
	int random_conditions;
	int only_score_item_playing;
	int check_score;
	int approve_score;
	int show_system;
	int show_prev;
	int play_mono_as_stereo;

	int valid_license;
	int no_score;

	/* portable window location information */
	Location admin_at;
	Location test_at;
};

#endif /* __CONFIG_H */