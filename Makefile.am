bin_PROGRAMS = healpixviewer

AUTOMAKE_OPTIONS = no-dependencies

BUILT_SOURCES = \
	src/ui_histogramwidget.h\
	src/ui_mainwindow.h \
	src/ui_mapoptions.h \
	src/ui_viewportmanager.h \
	src/ui_gridoptions.h \
	src/ui_coordinatesystem.h

healpixviewer_SOURCES = \
		src/icons.cpp \
		src/moc_facecache.cpp \
		src/moc_histogramviewer.cpp \
		src/moc_histogramwidget.cpp \
		src/moc_mainwindow.cpp \
		src/moc_maploader.cpp \
		src/moc_mapoptions.cpp \
		src/moc_gridoptions.cpp \
		src/moc_coordinatesystem.cpp \
		src/moc_mapviewer.cpp \
		src/moc_mapviewport.cpp \
		src/moc_overlaycache.cpp \
		src/moc_texturecache.cpp \
		src/moc_viewportmanager.cpp \
		src/moc_workspace.cpp \
		src/boundary.cpp \
		src/colormap.cpp \
		src/face.cpp \
		src/facecache.cpp \
		src/facevertices.cpp \
		src/fieldmap.cpp \
		src/grid.cpp \
		src/healpixmap.cpp \
		src/histogram.cpp \
		src/histogramviewer.cpp \
		src/histogramwidget.cpp \
		src/main.cpp \
		src/mainwindow.cpp \
		src/maploader.cpp \
		src/mapoptions.cpp \
		src/gridoptions.cpp \
		src/coordinatesystem.cpp \
		src/mapoverlay.cpp \
		src/mapviewer.cpp \
		src/mapviewport.cpp \
		src/overlaycache.cpp \
		src/polarizationvectors.cpp \
		src/tesselation.cpp \
		src/texture.cpp \
		src/texturecache.cpp \
		src/vertice.cpp \
		src/viewportmanager.cpp \
		src/workspace.cpp \
		src/roi.cpp \
		src/pixellutcache.cpp

app:
	rm -rf HealpixViewer.app
	mkdir -p HealpixViewer.app/Contents/MacOS
	mkdir -p HealpixViewer.app/Contents/Resources
	cp healpixviewer HealpixViewer.app/Contents/MacOS/
	cp Info.plist HealpixViewer.app/Contents
	cp healpixviewer.icns HealpixViewer.app/Contents/Resources
	cp -R colormaps HealpixViewer.app/Contents/Resources/colormaps
	macdeployqt HealpixViewer.app

src/icons.cpp :
	$(RCC) src/icons.qrc -o src/icons.cpp

src/ui_%.h : src/%.ui
	$(UIC) $< -o $@

src/moc_%.cpp : src/%.h
	$(MOC) $< -o $@

clean-local :
	-rm -rf src/moc_*
	-rm -f src/icons.cpp

EXTRA_DIST = \
	$(nodist_skyviewer_SOURCES:moc_%.cpp=%.h) \
	icons.qrc
