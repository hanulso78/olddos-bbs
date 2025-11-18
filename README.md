
README
-
This program aims to implement a BBS (Bulletin Board System) in the style popular in Korea during the 1990s. Currently, the program is being developed and operated on CentOS 6.9 (https://bbsweb.oscc.kr/).

This program is licensed under the LGPL.

INSTALL
-
For installation instructions, please refer to the INSTALL.txt file.

CONFIGURATION
-
Before installation, you will need the hanulso.cfg file. Please create it as follows.

```xml
<?xml version="1.0" encoding="euc-kr"?>
<hanulso>
	<name>TITLE</name>
	<database>
		<name>bbs</name>
		<host>localhost</host>
		<user>olddos</user>
		<password>password!!</password>
	</database>
	<sysop>
		<user>admin</user>	<!-- Admin -->
		<user>admin2</user>	<!-- Admin2 -->		
	</sysop>
	<article>
		<show_max_line>15</show_max_line>
		<ks_conversion>true</ks_conversion>
	</article>
	<level>
		<alias>1,일반회원</alias>
		<alias>2,특별회원</alias>
	</level>
</hanulso>
```

SCREENSHOT
-
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/7e9b6b06-dbe8-4587-a79e-c77295dfa7db" />
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/c1919b16-0487-4b2c-8026-2b2dafb58ec1" />
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/df5e2bb2-b220-4701-916e-d28b977600e2" />
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/a678b5da-d84f-4b62-908c-a467c53ffc83" />

