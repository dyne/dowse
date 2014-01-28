#!/bin/zsh

# name of this module
THIS=`basename $PWD`

# it's all conf!
module_setup() {

rm -f $PWD/$THIS.pl
cat <<EOF > $PWD/$THIS.pl
#!/usr/bin/env perl
use strict;
use warnings;
use lib '$PWD';
use HTTPSEverywhere;

\$|=1;

my \$he      = new HTTPSEverywhere( rulesets => ["$PWD/https-everywhere/src/chrome/content/rules"] );
my \$updated = time;

while(<>){
   my( \$url ) = /^(\S+)/;

   ## Re-read the rulesets if it has been more than an hour
     if( \$updated < time-3600 ){
	\$he->read();
	\$updated = time;
     }

   ## Converted version of the url
     my \$newurl = \$he->convert( \$url );

   if( \$url eq \$newurl ){
      print "\$url\n";
   } else {
      print "301:\$newurl\n";
   }
}
EOF
chmod +x $PWD/$THIS.pl

cat <<EOF >> $DOWSE/run/squid.conf
redirect_program $PWD/$THIS.pl
url_rewrite_children 5
EOF
}

# nop
module_start() { }
module_stop() { }

return 0
