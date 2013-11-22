.PHONY:	all clean clobber check

TARGET_VERSION=1.0.1e
TARGET_FOLDER=openssl-$(TARGET_VERSION)

##VERY ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF

all:
	@./buildall.pl build

clean:
	@rm -rf $(TARGET_FOLDER) CURRENT

check:
	@./checkall.pl

clobber:
	@rm -rf $(TARGET_FOLDER) CURRENT

