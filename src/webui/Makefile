
all:
	./map_assets > src/assetmap.h
	../kore/kore flavor prod
	../kore/kore build

run:
	../kore/kore run

debug:
	../kore/kore flavor dev
	./map_assets > src/assetmap.h
	cp -f conf/webui-debug.conf.dist conf/webui.conf
	../kore/kore build
	./webui -r -n -f

clean:
	rm -f webui 
	../kore/kore clean
