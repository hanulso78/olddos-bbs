CC = gcc -O2
CXX = g++ -O2  
STRIP = strip

CFLAGS = -I. -Wall

LINUX = $(shell echo `lsb_release -i | cut -d':' -f2`)
#$(info  $(LINUX))
ifeq ($(LINUX), CentOS)
	#LIBMYSQL=-lmysqlclient
	LIBMYSQL=`mysql_config --libs`
else ifeq ($(LINUX), AmazonAMI)
	LIBMYSQL=/usr/lib64/mysql57/libmysqlclient.so
else ifeq ($(LINUX), Ubuntu)
	LIBMYSQL=`mysql_config --libs`
endif

 
.SUFFIXES: .c .cpp .o 
.c.o: ; $(CC) -c $(CFLAGS) -o $*.o $<
.cpp.o: ; $(CXX) -c $(CFLAGS) -o $*.o $<

all: ctime main guest pass sysop weather memo biorhythm chattclient chattserver mbti_gs 

main: utility.o textutil.o database.o lineeditor.o fileeditor.o main.o \
	base64.o pugixml.o upload.o download.o write.o edit.o hangul.o \
	settings.o menu.o chatt.o news.o profile.o re.o
	$(CXX) -o ../bin/$@ main.o utility.o textutil.o database.o settings.o menu.o \
			lineeditor.o fileeditor.o upload.o download.o \
			write.o edit.o base64.o pugixml.o hangul.o chatt.o \
			news.o profile.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt
	$(STRIP) ../bin/$@
 
ctime: ctime.o 
	$(CC) -o ../$@ ctime.o 

guest: guest.o database.o utility.o textutil.o settings.o menu.o
	$(CXX) -o ../bin/$@ pugixml.o guest.o database.o utility.o textutil.o settings.o menu.o \
			$(LIBMYSQL) -lz -lpthread -lrt
	$(STRIP) ../bin/$@

pass: pass.o database.o utility.o textutil.o settings.o menu.o
	$(CXX) -o ../bin/$@ pugixml.o pass.o database.o utility.o textutil.o settings.o menu.o \
			$(LIBMYSQL) -lz -lpthread -lrt
	$(STRIP) ../bin/$@

sysop: sysop.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o sysop.o database.o utility.o textutil.o settings.o menu.o \
			$(LIBMYSQL) -lz -lpthread -lrt
	$(STRIP) ../bin/$@

weather: weather.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o weather.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt
	$(STRIP) ../bin/$@

biorhythm: biorhythm.o
	$(CC) -o ../bin/$@ biorhythm.o -lm
	$(STRIP) ../bin/$@

mbti_gs: mbti_gs.o
	$(CC) -o ../bin/$@ mbti_gs.o -lm
	$(STRIP) ../bin/$@

memo: memo.o
	$(CXX) -o ../bin/$@ pugixml.o memo.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt
	$(STRIP) ../bin/$@

chattclient: chattclient.o 
	$(CXX) -o ../bin/$@ chattclient.o -lpthread
	$(STRIP) ../bin/$@

chattserver: chattserver.o
	$(CXX) -o ../bin/$@ chattserver.o 
	$(STRIP) ../bin/$@

clean:
	$(RM) *.o

main.o : main.cpp
database.o : database.cpp
ctime.o : ctime.c
guest.o : guest.cpp
pass.o : pass.cpp
lineeditor.o : lineeditor.cpp
fileeditor.o : fileeditor.cpp
upload.o : upload.cpp
download.o : download.cpp
write.o : write.cpp
edit.o : edit.cpp
utility.o : utility.cpp
textutil.o : textutil.cpp
base64.o : base64.cpp
pugixml.o : pugixml.cpp
hangul.o : hangul.cpp
settings.o : settings.cpp
menu.o : menu.cpp
re.o : re.c
