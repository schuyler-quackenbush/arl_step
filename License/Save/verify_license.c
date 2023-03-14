#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
//#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <stdint.h>
#include <assert.h>

#define byte    	unsigned char
#define LINE_LEN 	128
#define MAX_LINES	128
#define LICENSE_LEN	4096

/* function prototypes */
int Base64Decode(char* b64message, unsigned char** buffer, size_t* length); 
int verify_it(const byte* msg, size_t msg_len, byte* signature, EVP_PKEY* pkey);

/* STEP Public Key 
 *
 * remove header and footer lines 
 * make base64 text as one string
 */
//-----BEGIN PUBLIC KEY-----
char *step_public_key = 
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAr1sN//o2d3KrE0v3KEkx"
"2haAuLkTftNKpvfcNUDH7MVZo4M1ktwk+/ubovUHV9D3NTbYuR7cJZSNk2b4wdMo"
"WYd2fAj3/sfX3W+OwtitgqcT1uRPPUcQA4v59iB0xrvqpDVmwZyyKDN8RhfvuM4I"
"HnWaVlUDkzekaYXhbZpkgEyn2uzvr269l9xqKSuXKsGC2qrQVmzjAYvbqtyWLhcW"
"E1HWOpcOW8Hyv35oEOx9Pk6OILjuWBiGhdxKKs5RJ9Q9zh8RZiHzdlLLhEMzCXvs"
"9o3jrKILeu36NCfnm+njJRA5H8hW1QJjEfTb+lzjcti0qddufBJ6KlOGBTvpeqTe"
"6wIDAQAB";
//-----END PUBLIC KEY-----


/* verify STEP license */
int verify_license( char *ifile )
{
	unsigned char *signSha256;
	size_t len_signSha256;
	int ret_val;
#define READ_LICENSE_FILE
#ifdef READ_LICENSE_FILE
	char user_info[LICENSE_LEN];
	char signSha256Base64[LICENSE_LEN];
	char line[LINE_LEN];
	FILE *fp;
 
  	if ( (fp = fopen(ifile, "r")) == NULL) {
    		fprintf(stderr, "ERROR: Cannot open %s\n", ifile);
    		return -1;
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
#else
	char *user_info = 
"* STEP Version 3.0\n"
"*\n"
"* Licensed to:\n"
"* Schuyler Quackenbush\n"
"*\n"
"* Created Tue Feb  2 13:24:24 UTC 2021\n";

/* no \n characters in base64 representation */
	char *signSha256Base64 = 
"WKSCgq2gJlcs8hu+GFu6QsmwTL7EY7LwLkh8mS+Qh28dayaluq3HK1nq0JqX0eQv"
"HqKzcHRiYo94z/wRsj/E984fhQ8z3zDmo2C28k5C/Lqf+Ckb+pqoyFTr/llDFl3a"
"6o8EmDllJMJNOSs02vGArI2mKK+qq5D+2y7Ezs1KiPD8SvFLlWBAuqsMZ4u5/V3g"
"XnPPpeTv6hFz0DNcPg2nC8ZO8XpZBjTrZl5JlJcEJXSg2SsB5v88ceTxOl2Y/3x1"
"zRO8vP3SWMNJs4/pGlzWst6hLZr2OKzeSoGSQhbTm5ksYky/+QsxLlgNYuzpYlr2"
"ucKiAruwUyOmdtwsr5yzLg==";
#endif
printf("%s", user_info);
printf("\n");
printf("%s", signSha256Base64);
printf("\n");

// RSA* public_key_;
// IO* bo = BIO_new(BIO_s_mem());
// BIO_write(bo, RsaPublicKey.c_str(),RsaPublicKey.length());
// PEM_read_bio_RSA_PUBKEY(bo, &public_key_, 0, 0 );
// BIO_free(bo);
// RSA_free(public_key_);

	/* decode base64 license signature */
	Base64Decode(signSha256Base64, &signSha256, &len_signSha256); 
// printf("\n%ld\n", len_signSha256);
// FILE *fp;
// fp = fopen("decoded_signSha256", "w");
// fwrite(signSha256, 1, len_signSha256, fp);
// fclose(fp);

#undef READ_PEM_FILE
#ifdef READ_PEM_FILE
	EVP_PKEY *pkey;
	fp = fopen("step_pubkey.pem", "r");
	pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
	fclose(fp);
#else
	/* read STEP public key from string */
	BIO *pkey_bio = NULL;
	EVP_PKEY *pkey = NULL;
	pkey_bio = BIO_new(BIO_s_mem());
	BIO_write(pkey_bio, step_public_key, strlen(step_public_key));
	PEM_read_bio_PUBKEY(pkey_bio, &pkey, NULL, NULL);
	BIO_free(pkey_bio);
#endif

    ret_val = verify_it((const unsigned char *)user_info, strlen(user_info), 
    	signSha256, pkey);

	return ret_val;
}