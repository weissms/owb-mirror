ROOT=../../../../
WEBCORE=../../../../WebCore/

CC=g++
WEBCORE_LIB=-L$(WEBCORE)/Projects/gdk/ -lwebcore-gdk
LDFLAGS=$(WEBCORE_LIB)  -L$(ROOT)/BALGlue/obj-gnu/ -lbalglue -L$(ROOT)/BAL/Configuration/obj-gnu/ -lbal


INCLUDES = -I$(WEBCORE)/bridge/win/

WEBCORE_GDK_CXXFLAGS = -I$(ROOT)/JavaScriptCore -I$(ROOT)/WebCore/. -I$(ROOT)/WebCore/include \
	-I$(ROOT)/WebCore/DerivedSources/WebCore -I$(ROOT)/WebCore/bindings/js -I$(ROOT)/WebCore/bridge \
	-I$(ROOT)/WebCore/editing -I$(ROOT)/WebCore/html -I$(ROOT)/WebCore/css -I$(ROOT)/WebCore/dom -I$(ROOT)/WebCore/kwq \
	-I$(ROOT)/WebCore/loader -I$(ROOT)/WebCore/page -I$(ROOT)/WebCore/platform -I$(ROOT)/WebCore/rendering -I$(ROOT)/WebCore/xml \
	-I$(ROOT)/WebCore/xpath -I$(ROOT)/WebCore/xpath/impl -I$(ROOT)/WebCore/kcanvas -I$(ROOT)/WebCore/kcanvas/device \
	-DAPPLE_CHANGES=1 -DKHTML_XSLT=1 -DLINUX -DUSE_CONSERVATIVE_GC=0 \
	-DHAVE_FUNC_ISNAN -D_THREAD_SAFE -pthread -I$(ROOT)/WebCore/platform/gdk \
	-DBUILDING_GDK__ -DBUILDING_CAIRO__ -Iplatform/gdk -Iplatform/cairo \
	-I$(ROOT)/WebCore/kcanvas/device/cairo -DXPATH_SUPPORT `xml2-config --cflags` \
	`xslt-config --cflags` `icu-config --cxxflags` `pkg-config --cflags freetype2` \
	`pkg-config --cflags fontconfig` `pkg-config --cflags gtk+-2.0` `curl-config \
	--cflags ` `pkg-config --cflags cairo` `pkg-config --cflags sqlite3` -fPIC -DPIC \
	-w 	
	
CFLAGS=-c -Wall $(INCLUDES) $(WEBCORE_GDK_CXXFLAGS) -D__OWB__

DUMPRENDERTREEDIR=.
DUMPRENDERTREESOURCES= $(DUMPRENDERTREEDIR)/DumpRenderTree.cpp

SOURCES=$(DUMPRENDERTREESOURCES)
OBJECTS=$(SOURCES:.cpp=.o) $(LOG_LIB)
EXECUTABLE=dumpRenderTree

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) -g $(CFLAGS) $< -o $@
	
clean_dump_render_tree :
	rm -rf $(DUMPRENDERTREEDIR)/*.o
	  
clean: clean_dump_render_tree
	rm -rf *.o *.exe $(EXECUTABLE)
