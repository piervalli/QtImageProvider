TEMPLATE = subdirs

SUBDIRS += \
  HttpImageProvider \
  example/App
example/App.depends = HttpImageProvider
