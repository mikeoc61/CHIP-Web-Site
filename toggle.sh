#!/bin/bash
###########################
#
# Single script to execute a cgi-bin command and return to calling web page 
#
# Version 1.1 - 11/10/17
#
# Author: Michael E. O'Connor <gmikeoc@gmail.com>
#
###########################

$(python ./toggle_light.py > /dev/null 2>&1)

echo "Content-type: text/html"
echo 
echo ""
echo "<html>"
echo "<title>Toggling Wemo Light Switch</title>"
echo "<head>"
echo "<h1>Toggling Wemo Light Switch...</h1>"
echo ""
sleep 5s
echo "<meta http-equiv=\"refresh\" content=\"2; url=$HTTP_REFERER\" />"
echo "</head>"
echo "<body>"
# echo "Called from: $HTTP_REFERER"
# echo "<pre>$(/usr/bin/env)</pre>"
echo ""
echo "</body>"
echo "</html>"
echo ""
