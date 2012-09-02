<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:dc="http://purl.org/dc/elements/1.1/" version="1.0">
<xsl:template match="//rss">
    <root>
    <name><xsl:value-of select="/rss/channel/title"/></name>
    <description><xsl:value-of select="/rss/channel/description"/></description>
    <path><xsl:value-of select="link"/></path>
    <xsl:for-each select="/rss/channel/item">
	    <name><xsl:value-of select="title"/></name>
	    <description><xsl:value-of select="description"/></description>
	    <path><xsl:value-of select="link"/></path>
	    <creator><xsl:value-of select="dc:creator"/></creator>
	    <date><xsl:value-of select="dc:date"/></date>
	    <xslt_plugin_end_tag>Default End Text</xslt_plugin_end_tag>
    </xsl:for-each>
    </root>
</xsl:template>
</xsl:stylesheet>
