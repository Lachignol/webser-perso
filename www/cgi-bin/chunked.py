#!/usr/bin/env python3
# filepath: www/cgi-bin/test_input.py
import sys
import os

print("Content-Type: text/html\n")
print("<html><body>")
print("<h1>CGI Input Test</h1>")

# Lire le body de la requête
content_length = os.environ.get('CONTENT_LENGTH', '0')
if content_length.isdigit() and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))
else:
    body = sys.stdin.read()  # Pour chunked, lit jusqu'à EOF

print(f"<p>Body reçu: <strong>{body}</strong></p>")
print(f"<p>Longueur: {len(body)} caractères</p>")
print("</body></html>")