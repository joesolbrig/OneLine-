For installation instructions, please refer to the INSTALL.txt file.  

Oneline Multibrowser/Application Launcher Alpha Release.

What: This a keystroke application Launcher that also lets you check email, do a full-text search of your harddisk, browse facebook, receive RSS feeds and browse the web. It's similar to Quick Silver for the Macintosh and even more similar to GnomeDo for Linux, especially the now-discontinued "Docky view" (though the Dock part isn't done yet). 

To begin: Launch Oneline. Type something, click on it or use the arrow-keys. It should be clear how you can browse files and other items. Pressing the alt-key shows which hotkeys activate which items. 

Why: Keystroke launchers exist in abundance. Oneline is different in that it aims to be both fast and powerful. It is full-featured file/webbrowser rather than a simple utility (for beter or worse). At the same time, Oneline aims to use incremental, background searches to avoid palpable delays and annoying, resource hogging "re-indexing". 
As a "universal client", Oneline knits together all your information. Your facebook friends and your email contacts can indexed by name, You should be able to see your friends' comments about a given link even if you receive the link as part of an RSS feed. Oneline can automatically scans the websites you visit often and finds RSS feeds for them. And much more!
But why is this important (besides being cool)? The appeal of Facebook and other social networking sites is that they do a similar process of knitting together and presenting data (Messages, data feeds, websites, etc) But this leave Facebook (or Google or etc) in possession of *your profrecences*, they know how you like data filtered. Oneline aims to bring your data filtering back to you. The most important and unique part of Oneline is a custom database for merging and weighing multiple data sources as well as keeping a cache of all your data on your own disk. 

Oneline is a tool to let you own your information. 

License: GPL 2.0+
Credits: I taken various bits of open source code from the web as need. 
* I began with the Launchy Application by Josh Karlin but the present code bares only the slight resemblance to it (for good or ill). 
* I incorporated the gnome-desktop item parsing code into my Gnome platform library since it's not distributed as a stable library. 
* I used code by toucan@textelectric.net as the starting point for btree-based custom database
* I copied and modified code from the Recol application to write the full text search plugin which interfaces with Recol. 
* Use the concept of Frecency heavily as coined by Mozilla. 

