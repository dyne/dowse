
all: cproto
	rm -f assets/*~ 
	./map_assets > src/assetmap.h
	../kore/kore flavor prod
	../kore/kore build

cproto:
	@echo "TODO Insert the automatic prototype creation "
run:
	../kore/kore run

debug: cproto
	rm -f assets/*~ 
	../kore/kore flavor dev
	./map_assets > src/assetmap.h
	cp -f conf/webui-debug.conf.dist conf/webui.conf
	../kore/kore build
	./webui -r -n -f

clean:
	rm -f webui 
	../kore/kore clean
