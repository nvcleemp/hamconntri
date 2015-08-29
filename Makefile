
SOURCES = hamconntri.c bitset.h\
          Makefile LICENSE README.md

all: build/hamconntri

clean:
	rm -rf build
	rm -rf dist

build/hamconntri: hamconntri.c lw_plane_graph.c
	mkdir -p build
	cc -o $@ -O4 -Wall $^

sources: dist/hamconntri-sources.zip dist/hamconntri-sources.tar.gz

dist/hamconntri-sources.zip: $(SOURCES)
	mkdir -p dist
	zip dist/hamconntri-sources $(SOURCES)

dist/hamconntri-sources.tar.gz: $(SOURCES)
	mkdir -p dist
	tar czf dist/hamconntri-sources.tar.gz $(SOURCES)
