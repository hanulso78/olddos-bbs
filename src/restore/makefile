CC = gcc
CXX = g++

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

 
all: restore 

pugixml.o: ../pugixml.cpp
	$(CXX) -c -o $@  $<

database.o: ../database.cpp
	$(CXX) -c -o $@  $<

restore.o: restore.cpp
	$(CXX) -c -o $@  $<

settings.o: ../settings.cpp
	$(CXX) -c -o $@  $<

utility.o: ../utility.cpp
	$(CXX) -c -o $@  $<

restore: pugixml.o database.o restore.o settings.o utility.o
	$(CXX) -o $@ restore.o pugixml.o database.o settings.o utility.o \
		$(LIBMYSQL) -lssl -lz -lpthread -lrt 

clean:
	$(RM) *.o

