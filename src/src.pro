TEMPLATE = subdirs
SUBDIRS = lib bin tests

bin.depends = lib
tests.depends = lib
