<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
   xmlns:dc="http://purl.org/dc/elements/1.1/" 
   xmlns:mime="http://www.freedesktop.org/standards/shared-mime-info" 
   xmlns:bookmark="http://www.freedesktop.org/standards/desktop-bookmarks"
   version="1.0">
<xsl:template match="//RecentFiles/RecentItem">
    <root>
    <xsl:for-each select="/RecentFiles/RecentItem">
	    <path><xsl:value-of select="URI"/></path>
	    <date><xsl:value-of select="Timestamp"/></date>
	    <parent>mime_type://<xsl:value-of select="mime-type"/></parent>
	    <parent>built-in://recently-used</parent>
	    <xslt_plugin_end_tag>Default End Text</xslt_plugin_end_tag>
    </xsl:for-each>
    </root>
</xsl:template>
</xsl:stylesheet>
