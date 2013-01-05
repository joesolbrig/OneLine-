<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
   xmlns:dc="http://purl.org/dc/elements/1.1/" 
   xmlns:mime="http://www.freedesktop.org/standards/shared-mime-info" 
   xmlns:bookmark="http://www.freedesktop.org/standards/desktop-bookmarks"
   version="1.0">
<xsl:template match="//xbel">
    <root>
    <xsl:for-each select="/xbel/bookmark">
	    <description><xsl:value-of select="description"/></description>
	    <path><xsl:value-of select="@href"/></path>
	    <date><xsl:value-of select="@visited"/></date>
	    <date-modified><xsl:value-of select="@modified"/></date-modified>
	    <parent>mime_type://<xsl:value-of select="info/metadata/mime:mime-type/@type"/></parent>
	    <parent>built-in://recently-used</parent>
	    <xslt_plugin_end_tag>Default End Text</xslt_plugin_end_tag>
    </xsl:for-each>
    </root>
</xsl:template>
</xsl:stylesheet>