#!/usr/bin/env python3
import os
import sys

# Récupérer PATH_INFO depuis l'environnement
path_info = os.environ.get('PATH_INFO', '')

print("Content-Type: text/html\n")
print("<html><body>")
print(f"<h1>PATH_INFO: {path_info}</h1>")

# Utiliser PATH_INFO pour router votre application
if path_info == '/user':
	print("<p>Page utilisateur</p>")
elif path_info.startswith('/user/'):
	user_id = path_info.split('/')[-1]
	print(f"<p>Utilisateur ID: {user_id}</p>")
elif path_info == '/admin':
	print("<p>Page admin</p>")
else:
	print(f"<p>Chemin inconnu: {path_info}</p>")

print("</body></html>")