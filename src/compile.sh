#!/usr/bin/env zsh

R=`pwd`
R=${R%/*}
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

source $R/zlibs/zuper
source $R/zlibs/zuper.init

case $1 in
    sup)
        [[ -r $R/sun/sup ]] || {
            pushd $R/src/sup
            make clean && make
            if [[ -r $R/run/sup ]]; then
                newsup=`sha256sum $R/src/sup/sup`
                newsup=${newsup[(w)1]}
                oldsup=`sha256sum $R/run/sup`
                oldsup=${oldsup[(w)1]}
                [[ "$newsup" = "$oldsup" ]] || {
                    cp $R/src/sup/sup $R/run/ }
            else
                cp $R/src/sup/sup $R/run/
            fi

            needsuid=0
            auth=`stat -c '%a %u %g' $R/run/sup`

            [[ "$auth[1]"    = "6" ]] || needsuid=$(( $needsuid + 1 ))
            [[ "$auth[(w)2]" = "0" ]] || needsuid=$(( $needsuid + 1 ))
            [[ "$auth[(w)3]" = "0" ]] || needsuid=$(( $needsuid + 1 ))

            [[ $needsuid = 0 ]] || {
                sudo="(root)# "; command -v sudo && sudo="sudo "
                act ""
                error "You need to suid the privilege escalation wrapper sup."
                act "Please issue the following commands:"
                cat <<EOF

${sudo}chown root:root $R/run/sup
${sudo}chmod +s $R/run/sup

EOF

                $R/run/sup -l
            }
        }

        ;;

    dnscrypt-proxy)
        [[ -r $R/run/dnscrypt-proxy ]] || {
            pushd $R/src/dnscrypt-proxy
            ./configure --without-systemd \
                && \
                make
            cp $R/src/dnscrypt-proxy/src/proxy/dnscrypt-proxy \
               $R/run
            popd
        }
        ;;

    pgl)
        [[ -r $R/run/pgl ]] || {
            pushd $R/src/pgl
            ./configure --without-qt4 --disable-dbus --enable-lowmem \
					--disable-networkmanager \
	                --prefix $R/run/pgl --sysconfdir $R/run/pgl/etc \
	                --with-initddir=$R/run/pgl/init.d \
                && \
                make \
                && \
                make install
            # install prefix is local to dowse
            popd
        }
        ;;

    dnscap)
        [[ -r $R/run/dnscap ]] || {
            pushd $R/src/dnscap
            ./configure \
                && \
                make \
                && \
                cp $R/src/dnscap/dnscap $R/run
            popd
        }
        ;;

    clean)

        make -C $R/src/dnscap clean
        rm $R/run/dnscap $R/run/dowse.so

        make -C $R/src/dnscrypt-proxy clean
        rm $R/run/dnscrypt-proxy

        rm -rf $R/run/pgl
        make -C $R/src/pgl    clean
        ;;
    *)
        print "usage; ./src/compile.sh [ pgl | dnscap | clean ]"
        ;;
esac
