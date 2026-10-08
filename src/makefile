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

all: ctime main guest pass sysop weather exchange lunar hero rain omok baseball mine blackjack tgsend lotto coin sun worldtime english love tojeong today fortune mudlink memo biorhythm chattclient chattserver mbti_gs aichat typing cafeimport zipcode zork1 gkermit yut

# 글쓰기 화면 편집기 pico (src/pico/pico, 한글 고침). ncurses-devel 이 필요하다.
# all 에는 넣지 않는다 (서버에 ncurses-devel 이 없으면 BBS 업데이트가 통째로 실패하므로).
.PHONY: pico
pico:
	cd pico/pico && $(MAKE)

main: utility.o textutil.o database.o lineeditor.o fileeditor.o main.o \
	base64.o pugixml.o upload.o download.o write.o edit.o hangul.o \
	settings.o menu.o chatt.o news.o profile.o re.o attendance.o telegram.o graffiti.o stats.o bbtags.o board_extra.o
	$(CXX) -o ../bin/$@ main.o utility.o textutil.o database.o settings.o menu.o attendance.o telegram.o graffiti.o stats.o bbtags.o board_extra.o \
			lineeditor.o fileeditor.o upload.o download.o \
			write.o edit.o base64.o pugixml.o hangul.o chatt.o \
			news.o profile.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@
 
ctime: ctime.o 
	$(CC) -o ../$@ ctime.o 

guest: guest.o database.o utility.o textutil.o settings.o menu.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o guest.o database.o utility.o textutil.o settings.o menu.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

pass: pass.o database.o utility.o textutil.o settings.o menu.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o pass.o database.o utility.o textutil.o settings.o menu.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

sysop: sysop.o database.o utility.o textutil.o settings.o menu.o re.o pugixml.o lineeditor.o
	$(CXX) -o ../bin/$@ pugixml.o sysop.o database.o utility.o textutil.o settings.o menu.o re.o lineeditor.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

weather: weather.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o weather.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

exchange: exchange.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o exchange.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

hero: hero.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o hero.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

rain: rain.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o rain.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

tgsend: tgsend.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o tgsend.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

lotto: lotto.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o lotto.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

coin: coin.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o coin.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

sun: sun.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o sun.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt -lm
	$(STRIP) ../bin/$@

worldtime: worldtime.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o worldtime.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

english: english.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o english.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

zork1: zork1/zork1.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o zork1/zork1.o utility.o 			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

zipcode: zipcode.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o zipcode.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

aichat: aichat.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o aichat.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

baseball: baseball.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o baseball.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

typing: typing.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o typing.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

yut: yut.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o yut.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt -lm
	$(STRIP) ../bin/$@

mine: mine.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o mine.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

blackjack: blackjack.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o blackjack.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

omok: omok.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o omok.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

today: today.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o today.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

fortune: fortune.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o fortune.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

mudlink: mudlink.o
	$(CXX) -o ../bin/$@ mudlink.o
	$(STRIP) ../bin/$@

love: love.o lunarcalc.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o love.o lunarcalc.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

tojeong: tojeong.o lunarcalc.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o tojeong.o lunarcalc.o database.o utility.o textutil.o settings.o menu.o re.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

lunar: lunar.o lunarcalc.o utility.o pugixml.o
	$(CXX) -o ../bin/$@ pugixml.o lunar.o lunarcalc.o utility.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

biorhythm: biorhythm.o
	$(CC) -o ../bin/$@ biorhythm.o -lm
	$(STRIP) ../bin/$@

mbti_gs: mbti_gs.o
	$(CC) -o ../bin/$@ mbti_gs.o -lm
	$(STRIP) ../bin/$@

# 네이버 카페 글 가져오기 (sysop 14 번)
cafeimport: cafeimport.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o
	$(CXX) -o ../bin/$@ pugixml.o cafeimport.o database.o utility.o textutil.o settings.o menu.o re.o 			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

memo: memo.o pugixml.o database.o utility.o textutil.o settings.o menu.o re.o lineeditor.o
	$(CXX) -o ../bin/$@ pugixml.o memo.o database.o utility.o textutil.o settings.o menu.o re.o lineeditor.o \
			$(LIBMYSQL) -lz -lpthread -lrt -lcrypt
	$(STRIP) ../bin/$@

chattclient: chattclient.o 
	$(CXX) -o ../bin/$@ chattclient.o -lpthread
	$(STRIP) ../bin/$@

chattserver: chattserver.o
	$(CXX) -o ../bin/$@ chattserver.o 
	$(STRIP) ../bin/$@

# 파일 주고받기 Kermit (G-Kermit 2.01). 받는 파일 이름에서 경로를 떼도록 고친 것 (src/gkermit/README)
# 원래 gkermit 은 보내는 쪽이 ../../hanulso.cfg 같은 이름을 보내면 받는 폴더 밖에 썼다
.PHONY: gkermit
gkermit:
	cd gkermit && $(MAKE) CC=gcc KFLAGS="-std=gnu89 -w" posix && cp gkermit ../../bin/gkermit

clean:
	$(RM) *.o zork1/*.o
	$(RM) gkermit/*.o gkermit/gwart gkermit/gproto.c gkermit/gkermit

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
