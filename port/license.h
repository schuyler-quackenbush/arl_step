#ifndef __LICENSE_H
#define __LICENSE_H

#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include "defineValues.h"

#define byte    	unsigned char
#define MAX_LINES	128
#define LICENSE_LEN	4096

/* helper functions */
int Base64Decode(char* b64message, unsigned char** buffer, size_t* length); 
int verify_signature(const byte* msg, size_t msg_len, byte* signature, EVP_PKEY* pkey);

class License {
public:
	License();
	~License();
	int validLicense( void );
	char *getTitle( void );
	char *getMessage( void );
	int checkLicense( char *license_filepath );
private:
	char myname[20];
	int  step_id;
	int  valid_license;
	char default_program_dir[FILE_LEN];
    char owner[LICENSE_LEN];
	char title[LAB_LEN];
	char message[MSG_LEN];
};

#endif /* __LICENSE_H */
