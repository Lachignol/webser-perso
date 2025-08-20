#!/usr/bin/ruby
print "Content-Type: text/html\r\n"
print "\r\n"
print "<h1>Ruby CGI Works!</h1>"
print "<p>This is a Ruby script</p>"
print "<p>Ruby version: #{RUBY_VERSION}</p>"
print "<p>Current time: #{Time.now}</p>"
