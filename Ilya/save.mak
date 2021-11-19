save.exe: save.obj save.res
	$(LINKER) %(GUIFLAGS) -OUT:save.exe save.obj\
	save.res $(GUILIBS)

save.obj: main.cpp save.h
	&(CC) &(CFLAGS) main.cpp

save.res : save.rc save.h
	$(RC) $(RCVARS) save.rc
