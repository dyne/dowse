
all:
	./map_assets > src/assetmap.h
	kore flavor dev
	kore build

run:
	kore run

debug:
	kore -c conf/webui.conf -d -r -n -f

clean:
	kore clean
