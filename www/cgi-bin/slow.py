#!/usr/bin/python3
# filepath: /home/aoger/Documents/common_core/fifth_circle/webserv/www/cgi-bin/slow.py

import time
import os

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>Slow CGI Test</h1>")
print("<p>Starting at:", time.strftime("%H:%M:%S"))

# Simuler traitement lent
time.sleep(5)

print("<p>Finished at:", time.strftime("%H:%M:%S"))
print("</body></html>")
