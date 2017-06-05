IN=$(shell ls *.sd |grep -v 00)
OUT=$(subst .sd,.pdf,$(IN))

all: $(OUT)

%.pdf : %.sd
	java -jar ../sdedit-4.2-beta8.jar -o $@ -t pdf $<
