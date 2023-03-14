#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <stdint.h>
#include <assert.h>
#include "license.h"

#define byte    	unsigned char
#define LINE_LEN 	128
#define MAX_LINES	128
//#define LICENSE_LEN	4096

/* function prototypes */
int verify_license( char *ifile, char *user_info );
int Base64Decode(char* b64message, unsigned char** buffer, size_t* length); 
int verify_signature(const byte* msg, size_t msg_len, byte* signature, EVP_PKEY* pkey);

/* STEP Public Key 
 *
 * remove header and footer lines 
 * make base64 text as one string
 */
char *step_public_key = 
"-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAr1sN//o2d3KrE0v3KEkx\n"
"2haAuLkTftNKpvfcNUDH7MVZo4M1ktwk+/ubovUHV9D3NTbYuR7cJZSNk2b4wdMo\n"
"WYd2fAj3/sfX3W+OwtitgqcT1uRPPUcQA4v59iB0xrvqpDVmwZyyKDN8RhfvuM4I\n"
"HnWaVlUDkzekaYXhbZpkgEyn2uzvr269l9xqKSuXKsGC2qrQVmzjAYvbqtyWLhcW\n"
"E1HWOpcOW8Hyv35oEOx9Pk6OILjuWBiGhdxKKs5RJ9Q9zh8RZiHzdlLLhEMzCXvs\n"
"9o3jrKILeu36NCfnm+njJRA5H8hW1QJjEfTb+lzjcti0qddufBJ6KlOGBTvpeqTe\n"
"6wIDAQAB\n"
"-----END PUBLIC KEY-----";

/* check STEP license */
int check_license( char *ifile, char *user_info )
{
	int ret_val;
	if (verify_license( ifile, user_info ) ) {
		ret_val=1;
	}
	else {
		ret_val=0;
		/* create Demo message */
        sprintf(user_info, "%s%s",
" This is a Demo version of STEP Version 3.0\n",
"\n"
" Please go to www.audioresearchlabs.com to obtain a STEP license\n");
	}
	return ret_val;
}

/* verify STEP license */
int verify_license( char *ifile, char *user_info )
{
	unsigned char *signSha256;
	size_t len_signSha256;
	int ret_val;
	char signSha256Base64[LICENSE_LEN];
	char line[LINE_LEN];
	FILE *fp;

  	if ( (fp = fopen(ifile, "r")) == NULL) {
    		fprintf(stderr, "ERROR: Cannot open %s\n", ifile);
    		return 0;
  	}
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
    ret_val = verify_signature((const unsigned char *)user_info, strlen(user_info), 
    	signSha256, pkey);
//    PEM_free(pkey);

	return ret_val;
}
