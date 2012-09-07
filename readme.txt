OneLine Multibrowser/Application Launcher Alpha Release.

What: This is a keystroke application Launcher that also lets you check email, do a full-text search of your hard disk, scan Facebook, receive RSS feeds and browse the web. It's similar to Quick Silver for the Macintosh and even more similar to GnomeDo for Linux, especially the now-discontinued "Docky view" (though the Dock part isn't done yet). 

For installation instructions: please refer to the INSTALL.txt file.  
To begin: Launch OneLine. Type something, click on it or use the arrow-keys. It should be clear how you can browse files and other items. Pressing the alt-key shows which hotkeys activate which items. 

Why: Keystroke launchers exist in abundance. OneLine is different in that it aims to be both fast and powerful. It aims to full-featured graphic-shell/file-browser/web-browser rather than a simple utility (for beter or worse). At the same time, OneLine aims to use incremental, background searches to avoid palpable delays and annoying, resource hogging "re-indexing". 
As a "universal client", OneLine knits together all your information. Your Facebook friends and your email contacts can indexed by name, You should be able to see your friends' comments about a given link even if you receive the link as part of an RSS feed. OneLine can automatically scans the websites you visit often and finds RSS feeds for them. And much more!
But why is this important (besides being cool)? The appeal of Facebook and other social networking sites is that they do a similar process of knitting together and presenting data (Messages, data feeds, websites, etc) But this leave Facebook (or Google or etc) in possession of *your preferences*, they know how you like data filtered. OneLine aims to bring your data filtering back to you. The most important and unique part of OneLine is a custom database for merging and weighing multiple data sources as well as keeping a cache of all your data on your own disk. 

OneLine is a tool to let you own your information. 

License: GPL 2.0+
Credits: I taken various bits of open source code from the web as need. 
* I began with the Launchy Application by Josh Karlin but the present code bares only a modest resemblance to it (for good or ill). 
* I incorporated the gnome-desktop item parsing code into my Gnome platform library since it's not distributed as a stable library. 
* I used code by toucan@textelectric.net as the starting point for the btree-based custom database
* I copied and modified code from the Recol application to write the full text search plugin which interfaces with Recol. 
* I use the concept of Frecency heavily. This term was coined by Mozilla corporation, thanks to them for the idea. 

	--> Thanks very much especially again to Josh Karlin for a nice skeleton to place my code over. 

