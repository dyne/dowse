Make your Dowse box
===================


Introduction
------------

Dowse can run on virtually any machine that can run GNU/Linux. The
simple process of installing Dowse on a machine that is already running
GNU/Linux is as follows:

1. Clone Dowse's git repository
2. Install Dowse's software dependencies
3. Compile Dowse
4. Install Dowse on the system


Preparation
-----------

We recommend creating a new unprivileged user for running Dowse. This
can be done by executing:

```
# useradd -m -s /bin/zsh dowse
# passwd dowse
```

The first command will create a user called `dowse` and set its shell to
`/bin/zsh`. With the second command (`passwd`) you should set a password
for the user. Although in certain cases it is not necessary.

From now on, we will log in as the dowse user and continue setting up
out box.


Clone Dowse's git repository
----------------------------

Get a shell as `dowse` by issuing:

```
# su - dowse
```

You should now be in dowse's home directory: `/home/dowse`.

Continue by cloning Dowse's git repository:

```
$ git clone https://github.com/dyne/dowse.git dowse-src
```

Once finished, enter the `dowse-src` directory and issue:

```
$ git submodule update --init
```

If no errors are reported, continue. Otherwise, rinse and repeat.


Install Dowse's software dependencies
-------------------------------------

Dowse's package dependencies are listed in `README.md` in the git
repository. The easiest way to install them on Debian-based systems is
as follows:

```
# grep '^zsh' README.md | xargs apt-get --yes --force-yes --no-install-recommends install
```

This command will install the dependencies automatically. If you are
running a non-Debian-based distribution, consult your package manager
for the correct package names.

If this step was successful, please continue :)


Compile Dowse
-------------

So once you've ensured you have Dowse's dependencies installed, you can
simply issue the compile command now:

```
$ make
```

This could take a while, depending on your CPU's speed and the amount of
memory you have available.

Hopefully there were no errors, and you're ready for the final step:


Install Dowse
-------------

While still in the git repository, issue:

```
# make install
```

and Dowse will be installed system-wide.
