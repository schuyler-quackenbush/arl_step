#create license file

USER_INFO=user_info.txt

case $# in
	1) user_name=$1;; 
	*) echo "Usage: $0 user_name"; exit;;
esac

today=`date -u`
echo "****************************************************************"	>$USER_INFO
echo "* STEP Version 3"		>>$USER_INFO
echo "*"				>>$USER_INFO
echo "* Licensed to: $user_name"	>>$USER_INFO
echo "*"				>>$USER_INFO
echo "* Date: $today"			>>$USER_INFO
echo "****************************************************************"	>>$USER_INFO

echo "License User:"
cat  $USER_INFO

#create signature in binary
openssl dgst -sha256 -sign step_privkey.pem -out sign.sha256 $USER_INFO
#convert to base64
openssl enc -base64 -in sign.sha256 -out sign.sha256.base64

#put it together
cat  $USER_INFO sign.sha256.base64 > license.txt

#clean up
rm -f $USER_INFO
rm -f sign.sha256 sign.sha256.base64
