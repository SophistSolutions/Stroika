##VERY ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF

.PHONY:	all clean clobber check

TARGET_VERSION=1.0.1f
TARGET_FOLDER=openssl-$(TARGET_VERSION)

all:
	@./buildall_.pl $(TARGET_VERSION)
	@make check

clean:
	@rm -rf $(TARGET_FOLDER) CURRENT

check:
	@./checkall_.pl

clobber:
	@rm -rf $(TARGET_FOLDER) CURRENT
