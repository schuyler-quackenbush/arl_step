#create public 
openssl genpkey -out step_privkey.pem -algorithm rsa

#extract from that the private key
openssl rsa -in step_privkey.pem -outform PEM -pubout -out step_pubkey.pem

