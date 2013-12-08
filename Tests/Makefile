#### SUPER PRELIM - REALLY just a hack to be able to run remote tests.

###barely a legit start...


TESTS=	Test01 Test02 Test03 Test04 Test05 Test06 Test07 Test08 Test09 Test10 Test11 Test12 Test13 Test14 Test15 Test16 Test17 Test18 Test19 Test20 Test21 Test22 Test23 Test24 Test25 Test26 Test27 Test28 Test29 Test30 Test31 Test32 Test33 Test34 Test35 Test36

REMOTE_MACHINE_AND_USERNAME	=	root@192.168.244.110


run-tests-remotely:
	for ti in $(TESTS); do \
		echo Running $$ti on $(REMOTE_MACHINE_AND_USERNAME)...; \
		scp ../Builds/DefaultConfiguration/$$ti $(REMOTE_MACHINE_AND_USERNAME):/tmp/$$ti; \
		ssh $(REMOTE_MACHINE_AND_USERNAME) /tmp/$$ti; \
	done
