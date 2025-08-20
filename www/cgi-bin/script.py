#!/usr/bin/env python3

import os

print("Content-Type: text/html")
print()

print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello depuis CGI Python !</h1>")
print("<p>QUERY_STRING = {}</p>".format(os.environ.get("QUERY_STRING", "")))
print("</body>")
print("</html>")
