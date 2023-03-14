#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <stdint.h>
#include <assert.h>
#include "license.h"

#define byte    	unsigned char
#define MAX_LINES	128
#define LICENSE_LEN	4096

/* external helper functions */
int Base64Decode(char* b64message, unsigned char** buffer, size_t* length); 
int verify_signature(const byte* msg, size_t msg_len, byte* signature, EVP_PKEY* pkey);

/* STEP Public Key 
 *
 * remove header and footer lines 
 * make base64 text as one string
 */
char *step_public_key = (char *)
"-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAr1sN//o2d3KrE0v3KEkx\n"
"2haAuLkTftNKpvfcNUDH7MVZo4M1ktwk+/ubovUHV9D3NTbYuR7cJZSNk2b4wdMo\n"
"WYd2fAj3/sfX3W+OwtitgqcT1uRPPUcQA4v59iB0xrvqpDVmwZyyKDN8RhfvuM4I\n"
"HnWaVlUDkzekaYXhbZpkgEyn2uzvr269l9xqKSuXKsGC2qrQVmzjAYvbqtyWLhcW\n"
"E1HWOpcOW8Hyv35oEOx9Pk6OILjuWBiGhdxKKs5RJ9Q9zh8RZiHzdlLLhEMzCXvs\n"
"9o3jrKILeu36NCfnm+njJRA5H8hW1QJjEfTb+lzjcti0qddufBJ6KlOGBTvpeqTe\n"
"6wIDAQAB\n"
"-----END PUBLIC KEY-----";

/* class constructor */
License::License( )
{
	strcpy(myname, "STEP");
	strcpy(default_program_dir, "C:/Program Files/ARL/Step/");	/* Windows */
	valid_license = 0;
    owner[0] = 0;
}

/* class desctructor */
License::~License() {
	;
}

/* member functions */
int License::validLicense( void )
{
	return valid_license == 1 ? 1 : 0;
}

char * License::getTitle( void )
{
	return title;
}

char * License::getMessage( void )
{
	return message;
}

int License::checkLicense( char *license_pathname )
{
	unsigned char *signSha256;
	size_t len_signSha256;
	char signSha256Base64[LICENSE_LEN];
	char line[LINE_LEN];
	char user_info[LICENSE_LEN];
	FILE *fp;

    if ( (fp = fopen(license_pathname, "r")) == NULL) {
  		/* no license file */
  		valid_license = 0;
        fclose (fp);
  	}
    else {
        /* initialize character strings to zero length */
        user_info[0] = 0;
        signSha256Base64[0] = 0;
        /* read lines from license file */
        for (int i=0; i<MAX_LINES; i++) {
            if (fgets(line, LINE_LEN, fp) == NULL) {
                //end of file
                break;
            }
            if (line[0] == '*') {
                /* user information */
                strcat(user_info, line);
                //search for line with license owner
                if ( strncmp("Licensed to:", &line[2], 12) == 0) {
                    strcpy(owner, &line[2]);
                }
            }
            else {
                /* base64 signature */
                int n = strlen(line);
                line[n-1] = 0; //remove \n at end of line
                strcat(signSha256Base64, line);
            }
        }
        fclose(fp);

        /* decode base64 license signature */
        Base64Decode(signSha256Base64, &signSha256, &len_signSha256);

        /* read STEP public key from string */
        BIO *pkey_bio = NULL;
        EVP_PKEY *pkey = NULL;
        pkey_bio = BIO_new(BIO_s_mem());
        BIO_write(pkey_bio, step_public_key, strlen(step_public_key));
        PEM_read_bio_PUBKEY(pkey_bio, &pkey, NULL, NULL);
        BIO_free(pkey_bio);

        /* verify signature against user_info */
        valid_license = verify_signature((const unsigned char *)user_info, strlen(user_info),
                                         signSha256, pkey);
        //    PEM_free(pkey);
        //    PEM_free(pkey_bio);
    }

    strcpy( title, 
		valid_license == 1 ? 
		"Welcome to STEP" : "About STEP" );
	strcpy( owner,
		valid_license == 1 ? 
		owner : 
		"This is a demo version, to purchase a complete\n"
		"version visit:   http://www.AudioResearchLabs.com" );

	sprintf(message, 
		"STEP - Subjective Testing and Evaluation Program\n"
        "Copyright (c) 2004, 2023 Audio Research Labs, LLC\n"
		"All Rights Reserved\n"
		"\n"
		"Version:    %d.%s\n"
		"%d Channel configuration\n"
		"\n"
		"%s\n\n",
		VERSION, RELEASE, MAX_CHN, owner );

	return valid_license;
}

