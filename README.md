
README
-
This program aims to implement a BBS (Bulletin Board System) in the style popular in Korea during the 1990s.  
Currently, the program is being developed and operated on CentOS 6.9 (https://bbsweb.oscc.kr/).

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
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/6823c6c3-4e6f-4154-ad5a-dd53cb5997a1" />
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/b4629232-faea-4082-b20a-02e2c7a6d644" />
<img width="850" height="739" alt="image" src="https://github.com/user-attachments/assets/4d12cd6f-3724-419b-b246-e38293afb83d" />
