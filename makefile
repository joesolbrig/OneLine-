PREFIX=/usr
SKINS_PATH=$(PREFIX)/share/oneline/skins
PLUGINS_PATH=$(PREFIX)/lib/oneline/plugins
SCRIPT_PATH=$(PREFIX)/lib/oneline/scripts
PLATFORMS_PATH=$(PREFIX)/lib/oneline/
DESKTOP_PATH=$(PREFIX)/share/applications
ICON_PATH=$(PREFIX)/share/pixmaps
#QMAKE=qmake-qt4
QMAKE=qmake
ONELINE_DIR=.oneline

DEFS=SKINS_PATH=\\\"$(SKINS_PATH)\\\" PLUGINS_PATH=\\\"$(PLUGINS_PATH)\\\" \
	PLATFORMS_PATH=\\\"$(PLATFORMS_PATH)\\\" SCRIPT_PATH=\\\"$(SCRIPT_PATH)\\\"

DIRS=. platforms/unix plugins/fileCatalog plugins/fullTextPlugins 


release:: 
	$(DEFS) $(QMAKE) oneline.pro && $(DEFS) make -f Makefile release
	cd platforms/unix && $(DEFS) $(QMAKE) unix.pro && $(DEFS) make release
	cd plugins/history && $(QMAKE) history.pro && make release
	cd plugins/fileCatalog && $(QMAKE) fileCatalog.pro && make release
	cd plugins/fullTextPlugin && $(QMAKE) fullTextPlugin.pro && make release
	cd plugins/fireFox && $(QMAKE) fireFox.pro && make release
	cd plugins/tags && $(QMAKE) tags.pro && make release
	cd plugins/testPlugin && $(QMAKE) testPlugin.pro && make release
	cd plugins/streamplugin && $(QMAKE) streamplugin.pro && make release
	
debug::
	$(DEFS) $(QMAKE) oneline.pro && $(DEFS) make -f Makefile debug
	cd platforms/unix && $(DEFS) $(QMAKE) unix.pro && make debug
	cd platforms/gnome && $(DEFS) $(QMAKE) gnome.pro && make debug
	cd plugins/fileCatalog && $(QMAKE) fileCatalog.pro && make debug
	cd plugins/fullTextPlugin && $(QMAKE) fullTextPlugin.pro && make debug
	cd plugins/history && $(QMAKE) history.pro && make debug
	cd plugins/fireFox && $(QMAKE) fireFox.pro && make debug
	cd plugins/tags && $(QMAKE) tags.pro && make debug
	cd plugins/testPlugin && $(QMAKE) testPlugin.pro && make debug
	cd plugins/streamplugin && $(QMAKE) streamplugin.pro && make debug
	
clean::
	$(QMAKE) oneline.pro && make -f Makefile clean
	cd platforms/unix && $(QMAKE) unix.pro && $(DEFS) make clean
	cd platforms/gnome && $(QMAKE) gnome.pro && $(DEFS) make clean
	cd plugins/history && $(QMAKE) history.pro && make clean
	cd plugins/fileCatalog && $(QMAKE) fileCatalog.pro && make clean
	cd plugins/fullTextPlugin && $(QMAKE) fullTextPlugin.pro && make clean
	cd plugins/fireFox && $(QMAKE) fireFox.pro && make clean
	cd plugins/tags && $(QMAKE) tags.pro && make clean
	cd plugins/testPlugin && $(QMAKE) testPlugin.pro && make clean
	cd plugins/streamplugin && $(QMAKE) streamplugin.pro && make clean

install_debug:: debug
	-install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 debug/oneline $(DESTDIR)$(PREFIX)/bin/oneline
	-install -d $(DESTDIR)$(PLATFORMS_PATH)
	install -m 644 debug/libplatform_unix.so $(DESTDIR)$(PLATFORMS_PATH)/
	-install -d $(DESTDIR)$(PLUGINS_PATH)
	install -m 644 debug/libplatform_gnome.so $(DESTDIR)$(PLATFORMS_PATH)/
	-install -d $(DESTDIR)$(PLUGINS_PATH)
	install -m 644 debug/plugins/*.so $(DESTDIR)$(PLUGINS_PATH)/ 
	-install -d $(DESTDIR)$(PLUGINS_PATH)/icons
	-install -d $(DESTDIR)$(SKINS_PATH)
	cp -r skins $(DESTDIR)$(PREFIX)/share/oneline/
	install -d $(DESTDIR)$(DESKTOP_PATH)/
	install -m 644 linux/oneline.desktop $(DESTDIR)$(DESKTOP_PATH)/
	install -d $(DESTDIR)$(ICON_PATH)/
	-install -m 644 "misc/Icon/oneline.png" $(DESTDIR)$(ICON_PATH)/oneline.png

install:: release
	-install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 release/oneline $(DESTDIR)$(PREFIX)/bin/oneline
	-install -d $(DESTDIR)$(PLATFORMS_PATH)
	install -m 644 release/libplatform_gnome.so $(DESTDIR)$(PLATFORMS_PATH)/
	-install -d $(DESTDIR)$(SCRIPT_PATH)/custom_actions
	-install -d $(DESTDIR)$(SCRIPT_PATH)/js
	-install -d $(DESTDIR)$(SCRIPT_PATH)/shell
	install -m 666 script/custom_actions/*.oneline $(DESTDIR)$(SCRIPT_PATH)/ 
	install -m 666 script/js/*.js $(DESTDIR)$(SCRIPT_PATH)/js
	install -m 666 script/js/*.css $(DESTDIR)$(SCRIPT_PATH)/js 
	install -m 666 script/shell/*.sh $(DESTDIR)$(SCRIPT_PATH)/shell
	-install -d $(DESTDIR)$(PLUGINS_PATH)
	install -m 666 release/plugins/*.so $(DESTDIR)$(PLUGINS_PATH)/ 
	-install -d $(DESTDIR)$(PLUGINS_PATH)/icons
	-install -d $(DESTDIR)$(SKINS_PATH)
	cp -r skins $(DESTDIR)$(PREFIX)/share/oneline/
	cp -r script $(DESTDIR)$(PREFIX)/share/oneline/
	install -d $(DESTDIR)$(DESKTOP_PATH)/
	install -m 666 linux/oneline.desktop $(DESTDIR)$(DESKTOP_PATH)/
	install -d $(DESTDIR)$(ICON_PATH)/
	-install -m 666 "oneline.ico" $(DESTDIR)$(ICON_PATH)/oneline.ico
	install -d -o`logname` ~/$(ONELINE_DIR)
	install -d -o`logname` ~/$(ONELINE_DIR)/generated_icons
	install -d -o`logname` ~/$(ONELINE_DIR)/generated_items
	install -d -o`logname` ~/$(ONELINE_DIR)/temp_for_preview
	install -d -o`logname` ~/$(ONELINE_DIR)/item_cache
	install -d -o`logname` ~/$(ONELINE_DIR)/styles
	install -m 666 -o`logname` styles/*.* ~/$(ONELINE_DIR)/styles
	install -m 666 -o`logname` ConfigurationFiles/*.ini ~/$(ONELINE_DIR)
	install -d -o`logname` ~/$(ONELINE_DIR)/cached_feeds

uninstall::
	-rm $(PREFIX)/bin/oneline
	-rm -rf $(PLUGINS_PATH)
	-rm -rf $(PLATFORMS_PATH)
	-rm -rf $(SKINS_PATH)
	-rm $(DESTDIR)$(DESKTOP_PATH)/oneline.desktop
	-rm $(DESTDIR)$(ICON_PATH)/oneline.ico
	-rm -rf ~/$(ONELINE_DIR)
	