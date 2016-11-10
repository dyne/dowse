all: cproto asset
	rm -f assets/*~
	../kore/kore flavor prod
	../kore/kore build

cproto:
	@echo "TODO Insert the automatic prototype creation "
run:
	../kore/kore run

asset src/assetmap.c:
	@echo "Creating assetmap.c" ; ./map_assets > src/assetmap.c

debug: cproto src/assetmap.c
	rm -f assets/*~
	rm -f assets/*~
	../kore/kore flavor dev
	cp -f conf/webui-debug.conf.dist conf/webui.conf
	../kore/kore build
##	./webui -r -n -f

clean:
	rm -f webui
	../kore/kore clean
