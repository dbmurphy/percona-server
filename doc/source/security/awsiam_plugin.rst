.. _awsiam_plugin:

===========================
 AWS IAM Authentication Plugin
===========================

Percona AWS IAM Authentication Plugin is a free and Open Source implementation of the *MySQL*'s authentication plugin.


Installation
============

This plugin requires manual installation because it isn't installed by default. :: 

 mysql> INSTALL PLUGIN auth_awsiam SONAME 'auth_awsiam.so';
 
After the plugin has been installed it should be present in the plugins list. To check if the plugin has been correctly installed and active :: 

 mysql> SHOW PLUGINS;
 ...
 ...
 | auth_awsiam                       | ACTIVE   | AUTHENTICATION     | auth_awsiam.so | GPL     |

Configuration
=============
No additional MySQL configuration is required to use the plugin , but it assume you have already configured the AWS CLI or have an instance profile with the necessary permissions to call the AWS STS API.

Creating a user
================

After the PAM plugin has been configured, users can be created with the PAM plugin as authentication method :: 

  mysql> CREATE USER 'newuser'@'localhost' IDENTIFIED WITH auth_awsiam;

This will create a user ``newuser`` that can connect from ``localhost`` who will be authenticated using the AWS IAM plugin. It assume the username is the name of the IAM Role the the token will be generated for.


Connecting to the server
========================

Connecting to MySQL
Connect to MySQL using an IAM role and session token::

  mysql -u iam_role_name -p -h mysql_host

Enter the session token when prompted for a password.

Security Considerations
=======================
* Ensure communication between the MySQL client and server is encrypted (use SSL/TLS).
* Do not pass session tokens directly on the command line to avoid exposure in shell history.
* The AWS Instance role or AssumedRole used by the MySQL server needs permission to call sts:GetCallerIdentity.

Known issues
============



Version Specific Information
============================
