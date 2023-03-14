#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defineValues.h"
#include "license.h"
#include "config.h"
#include "portControls.h"
#include "utils.h"

/* class constructor */
Config::Config()
{
	this->config_file[0] = 0;
	this->global_config_file[0] = 0;
}

/* class desctructor */
Config::~Config()
{
	;
}

/* member functions */

void
Config::initConfig(int valid_license, const char *config_file)
{
	int c;
	char *p;
	my_strncpy(this->my_name, STEP_NAME, LAB_LEN);
	/* defaults */
	//Do not initialize session_file, this has already been set in init_session()
	this->session_dir[0] = 0;
	this->config_file[0] = 0;
	this->valid_license = 0;
	this->no_score = 0;
	this->lab_id[0] = 0;
	this->listener_id[0] = 0;
	this->session_id[0] = 0;
	this->dev_idx = 0;
	this->dev_name[0] = 0;
	this->transition_mode = 0;
	this->lock_options = 0;

	this->random_trials = 0;
	this->random_conditions = 0;
	this->only_score_item_playing = 0;
	this->check_score = 0;
	this->approve_score = 0;
	this->play_mono_as_stereo = 0;
	this->show_system = 0;
	this->show_prev = 0;

	/* set license status */
	this->valid_license = valid_license;

	/* get session file directory path */
	my_strncpy(this->session_dir, this->session_file, FILE_LEN);
	p = (char *)PATH_SEP;
	c = *p;
	p = strrchr(this->session_dir, c);
	if (p == NULL) {
		showError((char*)"Bad Session Directory");
	}
	else {
        *p = '\0'; //NULL
	}
	/* assemble config file path */
	my_strncpy(this->config_file, session_dir, FILE_LEN);
	my_strncat(this->config_file, PATH_SEP, FILE_LEN);
	my_strncat(this->config_file, config_file, FILE_LEN);

	/* set SessionID */
	my_strncpy(this->session_id, ++p, LAB_LEN); //p set above, pointing to last path separator
	p = strrchr(this->session_id, '.');
	if (p == NULL) {
		showError((char*)"Bad Session File Name");
	}
	else {
        *p = '\0'; //NULL;
	}
}

void
Config::setConfig(int devN, char *devName)
{
	this->dev_idx = devN;
	my_strncpy(this->dev_name, devName, LAB_LEN);
}

int
Config::readConfig()
{
	FILE *fp;
	Config *p = this;
	char *word, *value, *s;
	char buf[256];

	if( (fp = fopen( p->config_file, "r" )) == 0 )
		return 0;

	while( fgets( buf, sizeof(buf), fp ) ) {
		word = buf;
		if( (value = strchr( word, '=' )) == 0 )
			continue;
		*value++ = 0;
		if( (s = strchr( value, '\n' )) == 0 )
			continue;
		*s = 0;
		if( strcmp( word, "lab_id" ) == 0 ) {
			strncpy( p->lab_id, value, LAB_LEN );
		}
		else if( strcmp( word, "listener_id" ) == 0 ) {
			strncpy( p->listener_id, value, LAB_LEN );
		}
//		else if( strcmp( word, "session_id" ) == 0 ) {
//			strncpy( p->session_id, value, LAB_LEN );
//		}
//		else if( strcmp( word, "audio_dev_name" ) == 0 ) {
//			strncpy( p->dev_name, value, LAB_LEN );
//		}
		else if( strcmp( word, "random_trials" ) == 0 ) {
			p->random_trials = atoi(value);
		}
		else if( strcmp( word, "random_conditions" ) == 0 ) {
			p->random_conditions = atoi(value);
		}
		else if( strcmp( word, "only_score_item_playing" ) == 0 ) {
			p->only_score_item_playing = atoi(value);
		}
		if (strcmp(word, "check_score") == 0) {
			p->check_score = atoi(value);
		}
		if( strcmp( word, "approve_score" ) == 0 ) {
			p->approve_score = atoi(value);
		}
		else if( strcmp( word, "show_system" ) == 0 ) {
			p->show_system = atoi(value);
		}
		else if( strcmp( word, "show_prev" ) == 0 ) {
			p->show_prev = atoi(value);
		}
		else if( strcmp( word, "play_mono_as_stereo" ) == 0 ) {
			p->play_mono_as_stereo = atoi(value);
		}
	}
	fclose( fp );
	return 1;
}

int
Config::writeConfig()
{
	char msg[256];
	FILE *fp;
	Config *p = this;

	if ( strlen(p->config_file) == 0 ) {
		return 0;
	}
	if( (fp = fopen( p->config_file, "w" )) == 0 ) {
		sprintf(msg, "Can't write STEP configuration to %s", p->config_file );
		showError( msg );
		return 0;
	}
	fprintf( fp, "lab_id=%s\n", p->lab_id );
	fprintf( fp, "listener_id=%s\n", p->listener_id );
//	fprintf( fp, "session_id=%s\n", p->session_id );
//	fprintf( fp, "audio_dev_name=%s\n", p->dev_name );
	fprintf( fp, "transition=%d\n", p->transition_mode );
	fprintf( fp, "random_conditions=%d\n", p->random_conditions );
	fprintf( fp, "random_trials=%d\n", p->random_trials );
	fprintf( fp, "only_score_item_playing=%d\n", p->only_score_item_playing );
	fprintf (fp, "check_score=%d\n", p->check_score);
	fprintf( fp, "approve_score=%d\n", p->approve_score );
	fprintf( fp, "show_system=%d\n", p->show_system );
	fprintf( fp, "show_prev=%d\n", p->show_prev );
	fprintf( fp, "play_mono_as_stereo=%d\n", p->play_mono_as_stereo );
	fprintf( fp, "admin_at=%d %d\n", p->admin_at.x, p->admin_at.y );
	fprintf( fp, "test_at=%d %d\n", p->test_at.x, test_at.y );
	fclose( fp );
	return 1;
}

int
Config::readGlobalConfig( char *appl )
{
	FILE *fp;
	int c;
	char *p, *word, *value, *s;
	char buf[256];

	/* defaults */
	this->admin_at.set = 1;
	this->test_at.set = 1;

	/* get step application directory */
	my_strncpy(this->global_config_file, appl, FILE_LEN);
	p = (char *)PATH_SEP;
	c = *p;
	p = strrchr(this->global_config_file, c);
	if (p == NULL) {
		showError((char *)"Bad Session Directory");
	}
	else {
        *p = '\0'; //NULL;
	}
	/* assemble global config file path */
	my_strncat(this->global_config_file, PATH_SEP, FILE_LEN);
	my_strncat(this->global_config_file, CONFIG_FILE, FILE_LEN);

	if( (fp = fopen( this->global_config_file, "r" )) == 0 )
		return 0;

	while( fgets( buf, sizeof(buf), fp ) ) {
		word = buf;
		if( (value = strchr( word, '=' )) == 0 )
			continue;
		*value++ = 0;
		if( (s = strchr( value, '\n' )) == 0 )
			continue;
		*s = 0;
		if( strcmp( word, "admin_at" ) == 0 ) {
			if( sscanf( value, "%d%d", &this->admin_at.x, &this->admin_at.y ) == 2 )
				this->admin_at.set = 1;
		}
		else if( strcmp( word, "test_at" ) == 0 ) {
			if( sscanf( value, "%d%d", &this->test_at.x, &this->test_at.y ) == 2 )
				this->test_at.set = 1;
		}
        else if( strcmp( word, "session_dir" ) == 0 ) {
            strlcpy(this->session_dir, value, FILE_LEN);
        }
    }
	fclose(fp);
	return 1;
}

int
Config::writeGlobalConfig( )
{
	char msg[256];
	FILE *fp;
	Config *p = this;

	if ( strlen(p->global_config_file) == 0 ) {
		return 0;
	}
	if( (fp = fopen( p->global_config_file, "w" )) == 0 ) {
		sprintf(msg, "Can't write STEP configuration to %s", p->global_config_file );
		showError( msg );
		return 0;
	}
	fprintf( fp, "admin_at=%d %d\n", p->admin_at.x, p->admin_at.y );
	fprintf( fp, "test_at=%d %d\n", p->test_at.x, test_at.y );
    fprintf(fp, "session_dir=%s\n", p->session_dir);
	fclose( fp );
	return 1;
}

