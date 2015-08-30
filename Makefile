
SOURCES = hamconntri.c bitset.h\
          Makefile LICENSE README.md

all: build/hamconntri build/hamconntri_path

clean:
	rm -rf build
	rm -rf dist

build/hamconntri: hamconntri.c lw_plane_graph.c
	mkdir -p build
	cc -o $@ -O4 -Wall $^

build/hamconntri_path: hamconntri_path.c lw_plane_graph.c
	mkdir -p build
	cc -o $@ -O4 -Wall $^

sources: dist/hamconntri-sources.zip dist/hamconntri-sources.tar.gz

dist/hamconntri-sources.zip: $(SOURCES)
	mkdir -p dist
	zip dist/hamconntri-sources $(SOURCES)

dist/hamconntri-sources.tar.gz: $(SOURCES)
	mkdir -p dist
	tar czf dist/hamconntri-sources.tar.gz $(SOURCES)
