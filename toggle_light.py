#####
#
# Python2 program to call IFTTT dirctly and activate Applets
#
# Author: Michael O'Connor
#
# Last Change: 11/16/17
#
######

import time
import urllib2

# Specify IFTTT supplied key from Webhooks
# Obtain key from: https://ifttt.com/services/maker_webhooks/settings

maker_key = 'XXXXXXXXXXXXXXXXXXXXXXXXXXXX'

# Function forms the required URL based on <EVENT-TYPE> corresponding to Applet Name

def ifttt_handler (event):
  url = 'https://maker.ifttt.com/trigger/%s/with/key/%s' % (event, maker_key)
  f = urllib2.urlopen(url)
  response = f.read()
  f.close()
#  return response

# Event must match name created in IFTTT Webhooks applet

event = 'Under_Stairs_Light'

def main():

    ifttt_handler(event)	

# Standard boilerplate to call the main() function.

if __name__ == '__main__':
  main()
