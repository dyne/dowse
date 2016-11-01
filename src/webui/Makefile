
all:
	./map_assets > src/assetmap.h
	kore flavor prod
	kore build

run:
	kore run

debug:
	kore flavor dev
	./map_assets > src/assetmap.h
	cp -f conf/webui-debug.conf.dist conf/webui.conf
	../kore/kore build
	./webui -d -r -n -f

clean:
	rm -f webui
	kore clean
