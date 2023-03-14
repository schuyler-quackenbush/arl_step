#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdint.h>
#include <assert.h>

#define byte    unsigned char

/*
 * this program is equivalent to
 * verify signature of user_info (from verify_license.sh)
 * openssl dgst -sha256 -verify step_pubkey.pem -signature sign.sha256 user_info.txt
 * or
 * openssl dgst -sha256 -verify public.pem -signature message.secret message.txt
 * where
 * msg is message.txt (the clear text)
 * signature is message.secret (the signature from message.txt)
 * pkey is the public.pem public key ( achieved using PEM_read_PUBKEY )
 *
  */

int verify_signature(const byte* msg, size_t msg_len, byte* signature, EVP_PKEY* pkey) {
    EVP_MD_CTX      *ctx;
    size_t          sig_len;
    int             ret_val;

    // ret_val = EXIT_SUCCESS;
    // ctx = NULL;

    ctx = EVP_MD_CTX_create();    
    const EVP_MD* md = EVP_get_digestbyname( "SHA256" );

    EVP_DigestInit_ex( ctx, md, NULL );
    EVP_DigestVerifyInit( ctx, NULL, md, NULL, pkey );
    EVP_DigestVerifyUpdate(ctx, msg, msg_len);

    sig_len = 256;            
    if ( EVP_DigestVerifyFinal( ctx, signature, sig_len ) ) {
        ret_val = 1;
    }
    else {
        ERR_print_errors_fp( stdout );
        ret_val = 0;
    }

    return ret_val;
}
