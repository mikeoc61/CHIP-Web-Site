#!/bin/dash
###########################
#
# Script to report various systems data via cgi-bin to a web browser
#
# For apache2, place into /usr/lib/cgi-bin directory and chmod +x
#
# Note since some of the commands run require root privledge be sure to run sudo visudo
# to enable sudo commands to run wihout providing an interactive password.
#
# 	e.g.: www-data ALL = NOPASSWD: /usr/lib/cgi-bin/pwrstat, /usr/sbin/i2cget
#	
# Version 1.1 - 11/10/17
#
# Author: Michael E. O'Connor <gmikeoc@gmail.com>
#
###########################

echo "Content-type: text/html"
echo ""
echo "<html><head><title>Info for $(hostname -s)"
echo "</title></head><body>"

echo "<h1>System information for host: $(hostname -s)</h1>"
echo ""

echo "<h2>Public IP Address</h2>"
echo "$(curl -s 'https://api.ipify.org')"

echo "<h2>Power / Battery Stats</h2>"
echo "<pre>$(sudo ./pwrstat) </pre>"

#read fuel gauge B9h
BATT_GAUGE_HEX=$(sudo i2cget -y -f 0 0x34 0xb9)

# Converts hex to decimal 
# MSB is 8 bits, LSB is lower 4 bits
BATT_GAUGE_DEC=$(($BATT_GAUGE_HEX))
echo "<pre>Battery gauge:	"$BATT_GAUGE_DEC"% </pre>"

echo "<h2>Wifi Networks</h2>"
#echo "<pre>$(nmcli -p device wifi list) </pre>"
echo "<pre>$(nmcli device wifi list)</pre>"

echo "<h2>Process / Memory Info</h2>"
echo "<pre>$(top -b -n 1 | head -n 20) </pre>"

echo "<h2>Disk Info:</h2>"
echo "<pre>$(df -h) </pre>"

echo "<h2>Current Users</h2>"
echo "<pre>$(pinky) </pre>"

echo "</body></html>"
