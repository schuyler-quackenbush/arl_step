#verify license

#separate clear text from hash
cat license.txt | grep "^*" 	> user_info.txt
cat license.txt | grep -v "^*" 	> sign.sha256.base64

#decode signature from base64 text to binary
openssl enc -base64 -d -in sign.sha256.base64 -out sign.sha256

#verify signature of user_info
openssl dgst -sha256 -verify step_pubkey.pem -signature sign.sha256 user_info.txt